#include <LittleFS.h> //this needs to be above M5Unified in order for PNGs to be loaded directly from file
#include <M5Unified.h>
#include "platform/display.h"
#include "platform/log.h"
#include <cstring>  //for strstr
#include <cstdlib>  //for atoi
#include <ArduinoJson.h>    //for image manifests

static M5Canvas screen(&M5.Display);     //offscreen copy of the screen to reduce flickering

static textdatum_t datum_for(Align align){
    switch (align) {
        case Align::center:  return textdatum_t::middle_center;
        case Align::topleft: return textdatum_t::top_left;
    }
    return textdatum_t::top_left;
}

int display_get_width(){
    return M5.Display.width();
}

int display_get_height(){
    return M5.Display.height();
}

void draw_color(uint32_t color){
    screen.fillScreen(color);
}

void draw_text(int x, int y, int size, uint32_t color, Align align, const char* text){
    screen.setTextColor(color);
    screen.setTextSize(size);
    screen.setTextDatum(datum_for(align));
    screen.drawString(text, x, y);
}

struct SpriteSlot{
    M5Canvas canvas;
    int xorig = 0;
    int yorig = 0;
    int frames = 1;
    int frame_width = 0;
};

const int MAX_MANIFEST_ENTRIES = 256;
const int MAX_LOADED_SPRITES = 64;
const uint32_t SPRITE_TRANSPARENT = 0xFF00FF; //magenta, never use this in art
const int MAX_PATH_LENGTH = 96;
const int MAX_NAME_LENGTH = 32;

struct ManifestEntry{
    char name[MAX_NAME_LENGTH];
    char path[MAX_PATH_LENGTH];
    int xorig = 0;
    int yorig = 0;
};

static ManifestEntry manifest_entries[MAX_MANIFEST_ENTRIES];
static int manifest_count = 0;

static SpriteSlot sprites[MAX_LOADED_SPRITES];
static int sprite_count = 0;    // how many slots are used

//this bit is to get the size of a png file - 
//png files stores width and height as 4 byte numbers, starting at byte 16, so that's where we look
static bool png_size(const char* path, int& w, int& h){
    File png_file = LittleFS.open(path, "r");   // "r" means read
    if (!png_file) return false; //don't bother if the file is missing
    uint8_t header [24];
    bool ok = png_file.read(header, 24) == 24; //read the first 24 bytes
    png_file.close();
    if (!ok) return false;
    w = (header[16] << 24) | (header[17] << 16) | (header[18] << 8) | header[19];
    h = (header[20] << 24) | (header[21] << 16) | (header[22] << 8) | header[23];
    return true;
}

static void load_manifest(const char* folder){
    char manifest_path[MAX_PATH_LENGTH];
    snprintf(manifest_path, sizeof(manifest_path), "/sprites/%s/sprites.json", folder);

    File manifest_file = LittleFS.open(manifest_path, "r");
    if (!manifest_file) {show_debug_message("no manifest: %s", manifest_path); return;}

    JsonDocument manifest;  //this variable holds the parsed json
    DeserializationError error = deserializeJson(manifest, manifest_file);  //okay so i'm shaky on this, but basically this allows us to return multiple values somehow: the parsed json in "manifest", and "error", which contains a judgment on the content of manifest? error or not error?
    manifest_file.close();
    if (error) {show_debug_message("bad json in %s: %s", manifest_path, error.c_str()); return;}

    int default_xorig = manifest["origin"][0] | 0;  //"|0" is "or zero" aka "use 0 if this value is missing"
        /*
        | 0: normally | is bitwise OR, same as in GM. ArduinoJson redefines it for its own types to mean "this, or if it's missing / the wrong type, use this default". Libraries are allowed to do that in C++ (it's called operator overloading). Here it means default_xorig = 56, or 0 if the folder has no origin.
        */

    int default_yorig = manifest["origin"][1] | 0;

    for (JsonPair entry : manifest["sprites"].as<JsonObject>()){    //loop over every sprite in the folder
        if (manifest_count >= MAX_MANIFEST_ENTRIES){
            show_debug_message("load_manifest: table full, skipping %s", entry.key().c_str());  //c_str is .toString()
            return;
        }

        const char* sprite_name = entry.key().c_str();
        JsonObject info = entry.value();
        const char* file = info["file"] | "";   //reads filename from json

        ManifestEntry& table_entry = manifest_entries[manifest_count];  //add the manifest entry as a table entry

        snprintf(table_entry.name, sizeof(table_entry.name), "%s", sprite_name);
        
        /*
        table_entry.name = sprite_name; won't compile, because C++ doesn't let you assign a whole array with =. Using snprintf with plain "%s" means "copy this text in, and don't overflow". sizeof(table_entry.name) works here (it gives 32, or whatever you chose) because name is a real array inside the struct, not a pointer.
        */

        if (file[0] == '\0'){   // if no "file" in the json, use name.png
            snprintf(table_entry.path, sizeof(table_entry.path), "/sprites/%s/%s.png", folder, sprite_name);   //use the sprite name plus ".png"
        } else {
            snprintf(table_entry.path, sizeof(table_entry.path), "/sprites/%s/%s", folder, file);   //use the file name
        }

        table_entry.xorig = info["origin"][0] | default_xorig;
        table_entry.yorig = info["origin"][1] | default_yorig;

        show_debug_message("%s: file='%s' origin=%d,%d", table_entry.name, table_entry.path, table_entry.xorig, table_entry.yorig);
        manifest_count++;

    } 
}


int sprite_add(const char* fname, int xorig, int yorig){
    if (sprite_count >= MAX_LOADED_SPRITES) {
        show_debug_message("sprite-add: sprite Count %d has exceeded max sprites %d", sprite_count, MAX_LOADED_SPRITES);
        return -1;
    }

    //read the filename to determine frame number
    int frame_count = 1;    //if the filename doesn't have "_strip", it defaults to 1 frame
    const char* strip = strstr(fname, "_strip");    //find "_strip" in the name
    if (strip != nullptr){
        frame_count = atoi(strip+6);    //read the number after it
    }
    
    //build the filepath
    char path[MAX_PATH_LENGTH];  //makes an empty text (variable) chars long to put the filepath into
    snprintf(path, sizeof(path), "/sprites/%s.png", fname);
    
    //get the png size
    int width = 0;
    int height = 0;
    if (!png_size(path, width, height)) {
        show_debug_message("sprite_add: couldn't read %s", path);
        return -1;
    }

    //set up the slot to draw the png into
    SpriteSlot& slot = sprites[sprite_count]; //chooses the next open slot. & makes this a nickname instead of making a copy
    slot.canvas.setPsram(true); //you can use slot.canvas and it's different from regular canvas because it's inside "slot"
    slot.canvas.createSprite(width, height);
    slot.canvas.fillSprite(SPRITE_TRANSPARENT);
    slot.canvas.drawPngFile(LittleFS, path, 0, 0);
    slot.xorig = xorig;
    slot.yorig = yorig;
    slot.frames = frame_count;
    slot.frame_width = width/frame_count;

    return sprite_count++; //returns the slot we used and adds 1 one to the number
    //works the same as "int index = sprite_count; sprite_count++"
}

void draw_sprite(int sprite, int subimg, int x, int y){
    if (sprite < 0 || sprite >= sprite_count) return;   //if the sprite is not loaded, don't continue

    SpriteSlot& slot = sprites[sprite]; //grabs the CURRENT slot, not the next slot
    int32_t left = x-slot.xorig;
    int32_t top = y-slot.yorig;
    int frame = subimg % slot.frames;   //this line makes the subimg wrap around to frame 0 after it finishes
    screen.setClipRect(left, top, slot.frame_width, slot.canvas.height());
    slot.canvas.pushSprite(&screen, left - frame * slot.frame_width, top, SPRITE_TRANSPARENT); //copy the slot onto the address of the canvas, excluding the transparency color
    screen.clearClipRect();
    
}

void display_init(){
    screen.createSprite(M5.Display.width(), M5.Display.height());
    LittleFS.begin();
    load_manifest("pets/chobo");
    load_manifest("ui/menu");
}

void display_push(){
    screen.pushSprite(0, 0);
}
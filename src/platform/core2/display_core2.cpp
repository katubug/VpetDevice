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

const char* sprite_folders[] = {"pets/chobo", "ui/menu"};
const int SPRITE_FOLDER_COUNT = sizeof(sprite_folders)/sizeof(sprite_folders[0]);       //the number of folders is equal to the size of the array in bytes, divided by the size of one array entry. this ONLY works when the array is visible at the file level (so if it's dynamic or defined elsewhere, no dice)

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
    bool in_use = false;
    int which_sprite = -1;
    int current_users = 0;
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

        show_debug_message("%s: path='%s' origin=%d,%d", table_entry.name, table_entry.path, table_entry.xorig, table_entry.yorig);
        manifest_count++;

    } 
}

static int find_manifest_entry(const char* sprite_name){
    for (int i = 0; i < manifest_count; i++){
        if (strcmp(manifest_entries[i].name, sprite_name) == 0 ){
            return i;
        }
    }
    return -1;
}

static int find_manifest_entry_by_path(const char* sprite_path){
    for (int i = 0; i < manifest_count; i++){
        if (strcmp(manifest_entries[i].path, sprite_path) == 0 ){
            return i;
        }
    }
    return -1;
}

int sprite_add(const char* name){
    int entry_index = find_manifest_entry(name);
    if (entry_index == -1){
        show_debug_message("sprite_add: no manifest entry for %s", name);
        return -1;
    }
    const ManifestEntry& entry = manifest_entries[entry_index]; //nickname for the row, same as SpriteSlot& slot

    for (int i = 0; i < MAX_LOADED_SPRITES; i++){
        if (sprites[i].in_use && sprites[i].which_sprite == entry_index){
            sprites[i].current_users++;
            return i;   //return JUST the current index. nothing fancy.
        } 
    }

    //read the filename to determine frame number
    int frame_count = 1;    //if the filename doesn't have "_strip", it defaults to 1 frame
    const char* strip = strstr(entry.path, "_strip");    //find "_strip" in the name
    if (strip != nullptr){
        frame_count = atoi(strip+6);    //read the number after it
    }
    
    //get the png size
    int width = 0;
    int height = 0;
    if (!png_size(entry.path, width, height)) {
        show_debug_message("sprite_add: couldn't read %s", entry.path);
        return -1;
    }

        int free_slot = -1;     // instantiate free slot but we haven't found one yet
    for (int i = 0; i < MAX_LOADED_SPRITES; i++){  //search all slots
        if (!sprites[i].in_use){        //check if the slot is in use
            free_slot = i;              //mark the slot as free and exit
            break;
        }
    }
    if (free_slot == -1){
        show_debug_message("sprite_add: no free slots (max %d)", MAX_LOADED_SPRITES);
        return -1;
    }

    //set up the slot to draw the png into
    SpriteSlot& slot = sprites[free_slot]; //chooses the next open slot. & makes this a nickname instead of making a copy
    slot.canvas.setPsram(true); //you can use slot.canvas and it's different from regular canvas because it's inside "slot"
    slot.canvas.createSprite(width, height);
    slot.canvas.fillSprite(SPRITE_TRANSPARENT);
    slot.canvas.drawPngFile(LittleFS, entry.path, 0, 0);
    slot.xorig = entry.xorig;
    slot.yorig = entry.yorig;
    slot.frames = frame_count;
    slot.frame_width = width/frame_count;
    slot.in_use = true;
    slot.current_users = 1;
    slot.which_sprite = entry_index;

    return free_slot; //returns the slot we used
}

void draw_sprite(int sprite, int subimg, int x, int y){
    if (sprite < 0 || sprite >= MAX_LOADED_SPRITES || !sprites[sprite].in_use) return;   //if the sprite is not loaded, don't continue

    SpriteSlot& slot = sprites[sprite]; //grabs the CURRENT slot, not the next slot
    int32_t left = x-slot.xorig;
    int32_t top = y-slot.yorig;
    int frame = subimg % slot.frames;   //this line makes the subimg wrap around to frame 0 after it finishes
    screen.setClipRect(left, top, slot.frame_width, slot.canvas.height());
    slot.canvas.pushSprite(&screen, left - frame * slot.frame_width, top, SPRITE_TRANSPARENT); //copy the slot onto the address of the canvas, excluding the transparency color
    screen.clearClipRect();
    
}

void sprite_delete(int sprite){
    if (sprite < 0 || sprite >= MAX_LOADED_SPRITES || !sprites[sprite].in_use) return;   //if the sprite is not loaded, don't continue

    SpriteSlot& slot = sprites[sprite]; //grabs the CURRENT slot, not the next slot
    slot.current_users--;
    if (slot.current_users == 0){
        slot.canvas.deleteSprite();
        slot.in_use = false;
    }
    

}

static void check_manifest_files(){
    for (int i = 0; i < manifest_count; i++){   //the loop runs while i is less than the manifest count
        const ManifestEntry& entry = manifest_entries[i];
        if (!LittleFS.exists(entry.path)){
            show_debug_message("check_manifest_files: %s has no file at %s", entry.name, entry.path);
        }
    }
}

static void check_unlisted_pngs(){
    for (int i = 0; i < SPRITE_FOLDER_COUNT; i++){
        char folder_path[MAX_PATH_LENGTH];
        snprintf(folder_path, sizeof(folder_path), "/sprites/%s", sprite_folders[i]);
        
        File folder = LittleFS.open(folder_path);   //same as opening file
        if (!folder || !folder.isDirectory()) continue; //not there? move on. continue skips the rest of the current pass and jumps to the next i

        File png_file = folder.openNextFile();
        while (png_file){   //becomes false when no files are left
            bool is_png = strstr(png_file.name(), ".png") !=nullptr;
            if (is_png && find_manifest_entry_by_path(png_file.path()) == -1){
                show_debug_message("check_unlisted_pngs: %s isn't in any manifest", png_file.path());
            }
            png_file.close();
            png_file = folder.openNextFile();   //move on to the next file
        }
        folder.close(); //close the folder once all done.
    }
}

void display_init(){
    screen.createSprite(M5.Display.width(), M5.Display.height());
    LittleFS.begin();
    for (int i = 0; i < SPRITE_FOLDER_COUNT; i++){
        load_manifest(sprite_folders[i]);
    }
    check_manifest_files();     //this goes at the end because the manifests have to be loaded first
    check_unlisted_pngs();
}

void display_push(){
    screen.pushSprite(0, 0);
}
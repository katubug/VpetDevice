#include <LittleFS.h> //this needs to be above M5Unified in order for PNGs to be loaded directly from file
#include <M5Unified.h>
#include "platform/display.h"
#include "platform/log.h"

static M5Canvas canvas(&M5.Display);     //offscreen copy of the screen to reduce flickering

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
    canvas.fillScreen(color);
}

void draw_text(int x, int y, int size, uint32_t color, Align align, const char* text){
    canvas.setTextColor(color);
    canvas.setTextSize(size);
    canvas.setTextDatum(datum_for(align));
    canvas.drawString(text, x, y);
}

void display_init(){
    canvas.createSprite(M5.Display.width(), M5.Display.height());
    LittleFS.begin();
    bool chobo = LittleFS.exists("/sprites/spr_chobo_base.png");
    show_debug_message("chobo exists: %d", chobo);
}

void display_push(){
    canvas.pushSprite(0, 0);
}

struct SpriteSlot{
    M5Canvas canvas;
    int xorig = 0;
    int yorig = 0;
};

const int MAX_SPRITES = 32;
const uint32_t SPRITE_TRANSPARENT = 0xFF00FF; //magenta, never use this in art

static SpriteSlot sprites[MAX_SPRITES];
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

int sprite_add(const char* fname, int xorig, int yorig){
    if (sprite_count >= MAX_SPRITES) {
        show_debug_message("sprite-add: sprite Count %d has exceeded max sprites %d", sprite_count, MAX_SPRITES);
        return -1;
    }
    
    //build the filepath
    char path[64];  //makes an empty text 64 chars long to put the filepath into
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

    return sprite_count++; //returns the slot we used and adds 1 one to the number
    //works the same as "int index = sprite_count; sprite_count++"
}

void draw_sprite(int sprite, int subimg, int x, int y){
    if (sprite < 0 || sprite >= sprite_count) return;   //if the sprite is not loaded, don't continue

    SpriteSlot& slot = sprites[sprite]; //grabs the CURRENT slot, not the next slot
    slot.canvas.pushSprite(&canvas, x - slot.xorig, y - slot.yorig, SPRITE_TRANSPARENT); //copy the slot onto the address of the canvas, excluding the transparency color
}
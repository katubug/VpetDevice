#pragma once

class Scene{
    public:
        virtual void update() {}    //virtual means that a child can override this.
        virtual void draw() {}      //we give these bodies instead of semicolons so they can have contents in the future

};
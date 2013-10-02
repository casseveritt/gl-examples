//
//   Copyright 2013 NVIDIA
//   Cass Everitt - Oct 1, 2013
//
//   Copyright 2013 Pixar
//
//   Licensed under the Apache License, Version 2.0 (the "Apache License")
//   with the following modification; you may not use this file except in
//   compliance with the Apache License and the following modification to it:
//   Section 6. Trademarks. is deleted and replaced with:
//
//   6. Trademarks. This License does not grant permission to use the trade
//      names, trademarks, service marks, or product names of the Licensor
//      and its affiliates, except as required to comply with Section 4(c) of
//      the License and to reproduce the content of the NOTICE file.
//
//   You may obtain a copy of the Apache License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the Apache License with the above modification is
//   distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//   KIND, either express or implied. See the Apache License for the specific
//   language governing permissions and limitations under the Apache License.
//


#include <GL/Regal.h>
#include <stdio.h>
#include <stdlib.h>


extern void initOsd();
extern void updateGeom();
extern void display();
extern int g_width, g_height, g_frame;

void orig_reshape(int width, int height) {
    g_width = width;
    g_height = height;
}

//
// Idle is called between frames, here we advance the frame number and update
// the procedural animation that is being applied to the mesh
//
void orig_idle()
{
    g_frame++;
    updateGeom();
}


int orig_main(int argc, char ** argv)
{
    initOsd();
    return 0;
}


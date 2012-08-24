#pragma once

#include "ofMain.h"
#include <vector>

class testApp : public ofBaseApp{
	public:


		struct palette_struct {
			ofPoint position;
			float target;
			ofRectangle rect;
			ofRectangle displayRect;
		};		

		struct region {  
    		float center;
    		float left;
    		float right;
    		float displayleft;
    		float displayright;
    		palette_struct palette;
    		ofColor color;

			bool operator < (const region& r) const
    		{
        		return (center < r.center);
    		}

    		bool inside(float v)
    		{
    			return (v>left && v<right);
       		}

		}; 


		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);


	private:

		bool tryCreatingNewRegion(float p, float h);
		void calculateRegionBoundaries();
		bool tryMovingPalette(palette_struct *r, const ofPoint delta);
		bool tryMovingRegion(region *r, const float delta);

		float minimumRegionWidth;
		float idealRegionWidth;
		float ergoHeight;

		vector<region> regions;
		
		region *selectedRegion;
		palette_struct *selectedPalette;

		ofFbo canvas;

		ofRectangle displayWall;
		ofPoint mouse;
		ofPoint oldMouse;

};

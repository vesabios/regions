#include "testApp.h"

//#define __GOOPY__

//--------------------------------------------------------------
void testApp::setup(){
	
	ofBackground(255,255,255);
	ofSetWindowTitle("Regions Example");
	ofSetFrameRate(60);

	displayWall.set(10, 10, 1280, 270);


	canvas.allocate(displayWall.width, displayWall.height);
	canvas.begin();
	ofBackground(255,255,255);
	//ofRect(0,0,displayWall.width, displayWall.height);
	canvas.end();
	

	selectedRegion = NULL;

	// these should be pulled from an external configuration source

	minimumRegionWidth = 0.122; // ~ 4ft on a 392" wall, 28x55"
	idealRegionWidth = 0.18; // ~ 6ft on a 392" wall, 28x55"

	ergoHeight = 0.2;

}

//--------------------------------------------------------------
void testApp::update() {

	int len = regions.size();
	for ( int i=0; i<len; i++ )
  	{
  		region *r = &regions[i];

  		r->displayleft += (r->left - r->displayleft)*0.4;
  		r->displayright += (r->right - r->displayright)*0.4;

  		float delta = r->palette.position.x - r->center;
		tryMovingRegion(r, delta * 0.04);

		palette_struct *p = &(r->palette);

		float paletteDelta = (p->target - p->position.x) * 0.03;

		float sgn = 0;
		if (paletteDelta!=0)
		{
			sgn = paletteDelta/abs(paletteDelta);	
		} 

		paletteDelta = min((float)0.001,(float)abs(paletteDelta));

		p->position.x += paletteDelta * sgn;

  	}


}

//--------------------------------------------------------------
void testApp::draw() {

  

	calculateRegionBoundaries();

	float x,y,l,r;	
	ofSetColor(255,255,255);

	canvas.draw(displayWall.x, displayWall.y, displayWall.width, displayWall.height);

	// draw regions
	ofEnableAlphaBlending();    // turn on alpha blending
	y = displayWall.height + displayWall.y + 10;

	vector<region>::iterator it;
  	for ( it=regions.begin() ; it < regions.end(); it++ )
  	{


  		l = (int)((*it).displayleft * displayWall.width);
  		r = (int)((*it).displayright * displayWall.width);

		ofFill();
		ofSetColor(0,0,0,18);

		ofRect(displayWall.x + l, displayWall.y, r-l, displayWall.height);


		ofNoFill();

  		if (selectedRegion == &(*it))
  		{
  			ofSetColor(0,0,255,128);
  		} else {
  			ofSetColor(255,0,0,32);
  		}

  		ofLine((displayWall.x + l) + 1, y, (displayWall.x + r) -1, y);
  		ofLine((displayWall.x + l) + 1, y-4, (displayWall.x + l) + 1, y);
  		ofLine((displayWall.x + r) - 1, y-4, (displayWall.x + r) - 1, y);

		ofSetColor(0,0,0,64);

  		ofLine((displayWall.x + l) + 1, displayWall.y + 50, (displayWall.x + l) + 1, (displayWall.y + displayWall.height) - 50);
  		ofLine((displayWall.x + r) - 1, displayWall.y + 50, (displayWall.x + r) - 1, (displayWall.y + displayWall.height) - 50);

  		int midp = (displayWall.height/2) + displayWall.x;

		ofFill();
  		ofTriangle((displayWall.x + l) + 1, midp-5, (displayWall.x + l)+6, midp, (displayWall.x + l) + 1, midp+5);
  		ofTriangle((displayWall.x + r) - 1, midp-5, (displayWall.x + r)-6, midp, (displayWall.x + r) - 1, midp+5);

	}

	ofDisableAlphaBlending();




	// draw centerlines

/*
    ofSetColor(255,0,0,32);    // red, 50% transparent

  	for ( it=regions.begin() ; it < regions.end(); it++ )
  	{
  		x = (*it).center * displayWall.width;
  		ofLine(displayWall.x + x, displayWall.x, displayWall.x + x, displayWall.y + displayWall.height);
	}

	ofDisableAlphaBlending();   // turn off alpha	
*/




	// draw palettes

	ofSetColor(128,128,128,128);
	ofFill();

  	for ( it=regions.begin() ; it < regions.end(); it++ )
  	{
  		ofPoint pos = (*it).palette.position;
  		ofRectangle rect = (*it).palette.rect;
  		rect = rect + pos;
  		rect.x *= displayWall.width;
  		rect.width *= displayWall.width;
  		rect.y *= displayWall.height;
  		rect.height *= displayWall.height;
  		rect.x += displayWall.x;
  		rect.y += displayWall.y;
  		(*it).palette.displayRect.set(rect);

  		ofRect(rect);

	}	


	// draw boundary

	ofSetColor(0,0,0);
	ofNoFill();
	ofRect(displayWall);


	ofDrawBitmapString("press 'c' to clear", 10, 300);

}


void testApp::calculateRegionBoundaries()
{

	// let's sort the vector linearly along the X axis!!
	sort(regions.begin(), regions.end()); 	
	
	int len = regions.size();


	float left = 0.0;
	float right = 1.0;

	// we will be processing regions from left to right across the display surface
  	for ( int i=0; i<len; i++ )
  	{
  		region *r = &regions[i];

  		float center = r->center;

  		// first we start out by setting the left and right
  		// edges to be the neighboring edges of other regions,
  		// or the edges of the wall if necessary.  		
  		if (len>1)
  		{
	  		if (i==0)
	  		{
	  			left = 0.0;
	  		} else {
	  			left = regions[i-1].right;
	  		}

	  		if (i==len-1) 
	  		{
	  			right = 1.0;
	  		} else {
	  			right = regions[i+1].left;
	  		}
		}

  		// if the distance between any two regions is great enough
  		// to allow for some empty space, let's make that happen!

		float idealLeft = center - (idealRegionWidth/2.0);
		idealLeft = max(idealLeft, left);

		if ((idealLeft-left) > minimumRegionWidth)
		{
			left = idealLeft;
		} else {

#ifdef __GOOPY__
			
			if (left>0)
			{
				region *leftRegion = &regions[i-1];
				float midpoint = (leftRegion->center + center) / 2.0;
				leftRegion->right = midpoint;
				left = midpoint;
			}
#endif
			
		}

		float idealRight = center + (idealRegionWidth/2.0);
		idealRight = min(idealRight, right);

		if ((right-idealRight) > minimumRegionWidth)
		{
			right = idealRight;
		} else {

#ifdef __GOOPY__
			if (right<1)
			{
				region *rightRegion = &regions[i+1];
				float midpoint = (rightRegion->center + center) / 2.0;
				rightRegion->left = midpoint;
				right = midpoint;
			}
#endif
			
		}

  		r->left = left;
  		r->right = right;

	}

}


bool testApp::tryMovingPalette(palette_struct *pal, const ofPoint delta)
{

	pal->position += delta;
	pal->target = pal->position.x;

}

bool testApp::tryMovingRegion(region *r, const float delta)
{

	float buffer;
	float newCenter = r->center + delta;

	if (delta<0)
	{
		buffer = (newCenter - r->left) - minimumRegionWidth*0.5;
		if (buffer<0)
		{
			r->center = r->left + minimumRegionWidth*0.5;
		} else {
			r->center += delta;
		}
	} else {
		buffer = (r->right - newCenter) - minimumRegionWidth*0.5;
		if (buffer<0)
		{
			r->center = r->right - minimumRegionWidth*0.5;
		} else {
			r->center += delta;
		}
	}


}

bool testApp::tryCreatingNewRegion(float p, float h)
{

	// let's create two variables which hold our boundaries.
	// we initialize them to the maximum, i.e., the physical edge
	// of the wall on the left and right.
	float leftEdge = 0.0;
	float rightEdge = 1.0;


	// before we can allow the creation of a new region
	// we have to make sure that it's ok to do so...

	// does it lie within the boundaries of the physical space?
	if (p<=0) return 1;
	if (p>=1) return 1;


	// we can't have a new region centered on the edge of the wall..
	// there has to be at least a small buffer zone. we will set that here.

	if (p<minimumRegionWidth/2.0)
	{
		p = minimumRegionWidth/2.0;
	} else if (p>(1.0 - (minimumRegionWidth/2.0))) {
		p = 1.0 - (minimumRegionWidth/2.0);
	}


	// does our proposed new region overlap an existing region?

	vector<region>::iterator it;
  	for ( it=regions.begin() ; it < regions.end(); it++ )
  	{
  		if ((*it).inside(p))
  		{
  			printf("oops: new region is inside another one...\n");
  			return 1;
  		}
	}


	// how close is our proposed region to an existing region?

	float closestLeft = 0.0;
	float closestRight = 1.0;

  	for ( it=regions.begin() ; it < regions.end(); it++ )
  	{
  		if (((*it).right > closestLeft) && ((*it).right < p))
		{
			closestLeft = (*it).right;
		}
  		if (((*it).left < closestRight) && ((*it).left > p))
		{
			closestRight = (*it).left;
		}
	}	

	if ((p-closestLeft)<minimumRegionWidth/2.0)
	{
		p = closestLeft + (minimumRegionWidth/2.0);
	}
	if ((closestRight-p)<minimumRegionWidth/2.0)
	{
		p = closestRight - (minimumRegionWidth/2.0);
	}


	// create a new region and add it to the stack
	region r;
	r.center = p;
  	r.displayleft = p;
  	r.displayright = p;
  	r.left = closestLeft;
  	r.right = closestRight;

  	r.color.setHsb( ofRandom(0,255),255,255);

  	float palHeight = h;

  	palHeight -= 0.1;
  	palHeight = max(palHeight, (float)0.2);

  	r.palette.position.set(p, palHeight);
  	r.palette.target = p;
  	r.palette.rect.set(-0.02, -0.015, 0.04, 0.03);

	regions.push_back(r);

	// let's sort the vector linearly along the X axis!!
	sort(regions.begin(), regions.end()); 

	calculateRegionBoundaries();

}

//--------------------------------------------------------------
void testApp::keyPressed(int key) {

	if (key == 'c') {

		regions.clear();
		canvas.begin();
		ofBackground(255,255,255);
		//ofRect(0,0,displayWall.width, displayWall.height);
		canvas.end();		

	} else if (key == 'a') {

		if (displayWall.inside(mouse.x, mouse.y)) {

			// adjust for displayWall screen offset
			int x = mouse.x - displayWall.x;
			int y = mouse.y - displayWall.y;

			// normalize x to [0..1]
			float xn = (float)x / (float)displayWall.width;	
			float yn = (float)y / (float)displayWall.height;	

			tryCreatingNewRegion(xn, yn);

		}

	}

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {

	mouse.set(x, y);

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {

	if (selectedPalette)
	{
		int dx = x - oldMouse.x;
		int dy = y - oldMouse.y;
		ofPoint delta = ofPoint((float)dx / displayWall.width, (float)dy / displayWall.height);
		tryMovingPalette(selectedPalette, delta);

	} else {



		// let's make our toolbar follow the action!

		float mx = (float)(x - displayWall.x) / (float)displayWall.width;

		vector<region>::iterator it;
	  	for ( it=regions.begin() ; it < regions.end(); it++ )
	  	{
	  		if ((*it).inside(mx))
	  		{
				palette_struct *p = &it->palette;
				p->target += (mx - p->target) * 0.1;


				// actually draw into our canvas...

				canvas.begin();
				ofSetColor(it->color);
				ofLine(x - displayWall.x,y - displayWall.y,oldMouse.x - displayWall.x, oldMouse.y - displayWall.y);
				canvas.end();				

	  		}
		}		

	}

	oldMouse.set(x, y);	

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {

	vector<region>::iterator it;
  	for ( it=regions.begin() ; it < regions.end(); it++ )
  	{
  		if ((*it).palette.displayRect.inside(x,y))
  		{
  			selectedPalette = &it->palette;
  			//selectedRegion = &(*it);
  		}
	}

	mouse.set(x,y);
	oldMouse.set(x, y);	

	// if we haven't selected a palette to drag, let's try opening a region

	if (!selectedPalette) {

		if (displayWall.inside(mouse.x, mouse.y)) {

			// adjust for displayWall screen offset
			int x = mouse.x - displayWall.x;
			int y = mouse.y - displayWall.y;

			// normalize x to [0..1]
			float xn = (float)x / (float)displayWall.width;	
			float yn = (float)y / (float)displayWall.height;	

			tryCreatingNewRegion(xn, yn);

		}	
	}

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {

	selectedRegion = NULL;
	selectedPalette = NULL;	

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
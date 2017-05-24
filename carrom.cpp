	#include <iostream>
	#include <cmath>
	#include <GL/glut.h>
	#include <string.h>
	#include <sys/time.h>
	#include <cstdio>	

	using namespace std;

	#define PI 3.141592653589
	#define DEG2RAD(deg) (deg * PI / 180)

	void drawScene();
	void initRendering();
	void handleResize(int w, int h);
	void handleKeypress1(unsigned char key, int x, int y);
	void handleKeypress2(int key, int x, int y);
	void handleMouseclick(int button, int state, int x, int y);
	void drawBall(float,int);
	void update(int value);
	void mouse(int button, int state, int x, int y);
	void motion(int x, int y);
	void mouse_use(int btn,int state,int x,int y);


	int clicked = 0;
	float box_len = 5.0f;
	float box2_len = 9.0f;
	float point_x = 0.0f;
	float point_y = 0.0f;
	float amt = 0.0f;
	float angle = (float)(PI/2); 
	float mag = 0.0f ; 
	float friction = 0.97f ;
	int cscore = 30;
	bool mouseleftdown = false;
	int mousex, mousey; 
	timeval t1;

class coin;
	void drawBox(float len) {
	   
	    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	    glBegin(GL_QUADS);
	    glVertex2f(-len / 2, -len / 2);
	    glVertex2f(len / 2, -len / 2);
	    glVertex2f(len / 2, len / 2);
	    glVertex2f(-len / 2, len / 2);
	    glEnd();
	    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
		
	void drawBall(float rad,int type) {
	    
	   //glBegin(GL_TRIANGLE_FAN);
	    if(type == 1){
	    glLineWidth(2.0f);
	    glBegin(GL_LINE_LOOP);
	    
	    }   
	    else if(type == 0)
	    glBegin(GL_TRIANGLE_FAN);
	   for(int i=0 ; i<360 ; i++) {
	       glVertex3f(rad * cos(DEG2RAD(i)), rad * sin(DEG2RAD(i)), 0.0f);
	   }
	   glEnd();

	}

class ball {
	protected:
		

	public:
		float centre_x;
		float centre_y;
		float vel_x;
		float vel_y;
		float rad;
		ball(float x, float y, float radius)
		{
			centre_x = x;
			centre_y = y;
			vel_x = 0.0f;
			vel_y = 0.0f;
			rad = radius;

		}
		void draw(float r, float g, float b){
			glColor3f(r, g, b);
    		glTranslatef(centre_x, centre_y, 0.0f);    		
			
			drawBall(rad,0);
			glColor3f(0.0f, 0.0f, 0.0f);
			glLineWidth(2.0f);
	    
	   		drawBall(rad,1);
	    

		}

		void redspeed()
		{
			if(vel_y!=0 && abs(vel_x) < 1e-2*abs(vel_x/vel_y))
				vel_x = 0;
			else if(vel_y==0 && abs(vel_x) < 1e-2)
				vel_x = 0;
			else vel_x = friction*vel_x;

			if(abs(vel_y) < 0.01)
				vel_y = 0;
			else vel_y = friction*vel_y;
		}

		void pos_update(float box_len) {
			
			//reduce the speed of ball
			//redspeed();
			if(vel_y!=0 && abs(vel_x) < 0.01*abs(vel_x/vel_y))
				vel_x = 0;
			else if(vel_y==0 && abs(vel_x) < 0.01)
				vel_x = 0;
			else vel_x = friction*vel_x;

			if(abs(vel_y) < 0.01)
				vel_y = 0;
			else vel_y = friction*vel_y;
			// Handle ball collisions with box

			if(centre_x + rad > box_len / 2) {
				vel_x *= -friction;
				centre_x = box_len / 2 - rad;
			}

			if(centre_x - rad < - box_len / 2) {
				vel_x *= -friction;
				centre_x = - box_len / 2 + rad;
			}
			if(centre_y + rad > box_len / 2) {
				vel_y *= -friction;
				centre_y = box_len / 2 - rad;
			}

			if(centre_y - rad < - box_len / 2) {
				vel_y *= -friction;
				centre_y = - box_len / 2 + rad;
			}
			
			// Update position of ball
			centre_x += vel_x;
			centre_y += vel_y;
		}
		
		float getx() {return vel_x; }

		float gety() {return vel_y; }

		void setx(float a){vel_x += a;}

		void sety(float a){vel_y += a;}

		float getcentx(){return centre_x ;}
		float getcenty(){return centre_y;}

		
};
class striker : public ball {
	
	

	public:

		striker(float x, float y, float radius) : ball(x, y, radius)
		{
		}
		
		void shit_pos(float x) 
		{ 
			centre_x += x; 
		}
		void assignpos(float x){
			centre_x = x;
		}

		void ballback(float y) 
		{ 
			if(centre_x<(-1.45f) || centre_x>1.45f)
				centre_x = 0.0f;
			centre_y = y;
			vel_x = 0.0f;
			vel_y = 0.0f;
		}
		
		void coinmake()
		{
			draw(0.0f, 0.0f, 1.0f);
		}

		void linemake() 
		{
    //		glTranslatef(centre_x, centre_y, 0.0f);
    		glColor3f(0.0f, 1.0f, 1.0f);
			glBegin(GL_LINES);
			glVertex3f(0.0f, 0.0f,0.0f);
			glVertex3f((0.65f)*cos(angle), (0.65f)*sin(angle), 0.0f);
			glEnd();
		
		}

		void impulse() 
		{
			vel_x = (float)mag*cos(angle);
			vel_y = (float)mag*sin(angle);
		}


		friend void coinwithstriker(striker &c1, coin &c2);
};


class coin : public ball {
	private:
		int score;
		int type; //1 white, 0 black, 2 red
		int exists;	
	public:
		int added ;
		coin(int t, float x, float y, float radius) : ball(x, y, radius)
		{
			type = t;
			if(type == 2)
				score = 50;
			else score = 10;  
			exists = 0;
			added = 0 ;
		}
		void coinmake()
		{
			if(type == 1)
				draw(1.0, 1.0, 1.0);
			else if(type == 0) 
				draw(0, 0, 0);
			else draw(1.0, 0, 0);
		}
		int getexists(){return exists ;}
		
		void setexists(){exists = 1 ;}
		
		int getscore(){return score ;}

		int gettype(){return type ;}


		friend void coinwithcoin(coin &c1, coin &c2);
		friend void coinwithstriker(striker &c1, coin &c2);
};




class board {
		private:
			float len;
		public:
			board(float box_len){
				len = box_len;
			}
			void draw_layout()
			{
	    		glTranslatef(0.0f, 0.0f, -8.0f);
	    		glColor3f(0.45f, 0.23f, 0.14f);
	   		    drawBox(box_len);
	   	 //another box
	    	 glPushMatrix();
	   		 glTranslatef(0.0f, 0.0f, -8.0f);
	    	 glColor3f(0.95f, 0.86f, 0.70f);
	   		 drawBox(box2_len);
	    	 glPopMatrix();
	    	 makepockets();
	    	 design();
		    
			}
			void makepockets(){
	    	 		//pocket top right
	    	 glPushMatrix();
	    	 glTranslatef(4.1f, 4.1f, -8.0f);
	   		 glColor3f(0.6f, 0.3f, 0.0f);
	    	 drawBall(0.4f,0);
	   	     glPopMatrix();
	   		 //pocket top left
		   	glPushMatrix();
		    glTranslatef(-4.1f, 4.1f, -8.0f);
		    glColor3f(0.6f, 0.3f, 0.0f);
		    drawBall(0.4f,0);
		    glPopMatrix();
		    //pocket bottom right
		    glPushMatrix();
		    glTranslatef(4.1f, -4.1f, -8.0f);
		    glColor3f(0.6f, 0.3f, 0.0f);
		    drawBall(0.4f,0);
		    glPopMatrix();
		    //pocket bottom left
		    glPushMatrix();
		    glTranslatef(-4.1f, -4.1f, -8.0f);
		    glColor3f(0.6f, 0.3f, 0.0f);
		    drawBall(0.4f,0);
		    glPopMatrix();
	    	 }
	    	void design(){
	    	 //central circle for balls
		    glPushMatrix();
		    glTranslatef(0.0f, 0.0f, -8.0f);
		    glColor3f(0.0f, 0.0f, 0.0f);
		    drawBall(1.6f,1);
		    glTranslatef(4.2f, 5.1f, -8.0f);
		    drawBall(0.25f,1);
		    glColor3f(1.0f, 0.0f, 0.0f);
		    drawBall(0.24f,0);
			glPopMatrix();	    
		    glPushMatrix();
		    glLineWidth(2.0f);
		    glColor3f(0.0f, 0.0f, 0.0f);
		    glTranslatef(2.8f, -3.40f, -8.0f);
		    drawBall(0.18f,1);
		    glColor3f(1.0f, 0.0f, 0.0f);
		    drawBall(0.17f,0);

		    glColor3f(0.0f, 0.0f, 0.0f);
		    glTranslatef(-5.6f, 0.0f, 0.0f);
		    drawBall(0.18f,1);
		    glColor3f(1.0f, 0.0f, 0.0f);
		    drawBall(0.17f,0);

		    glColor3f(0.0f, 0.0f, 0.0f);
		    glTranslatef(0.0f, 6.8f, 0.0f);
		    drawBall(0.18f,1);
		    glColor3f(1.0f, 0.0f, 0.0f);
		    drawBall(0.17f,0);
		    
		    glColor3f(0.0f, 0.0f, 0.0f);
		    glTranslatef(-0.6f, -0.58f, 0.0f);
		    drawBall(0.18f,1);
		    glColor3f(1.0f, 0.0f, 0.0f);
		    drawBall(0.17f,0);

		    glColor3f(0.0f, 0.0f, 0.0f);
		    glTranslatef(0.0f, -5.6f, 0.0f);
		    drawBall(0.18f,1);
		    glColor3f(1.0f, 0.0f, 0.0f);
		    drawBall(0.17f,0);

		    glColor3f(0.0f, 0.0f, 0.0f);
		    glTranslatef(6.8f, 0.0f, 0.0f);
		    drawBall(0.18f,1);
		    glColor3f(1.0f, 0.0f, 0.0f);
		    drawBall(0.17f,0);
             
            glColor3f(0.0f, 0.0f, 0.0f);
		    glTranslatef(0.0f, 5.6f, 0.0f);
		    drawBall(0.18f,1);
		    glColor3f(1.0f, 0.0f, 0.0f);
		    drawBall(0.17f,0);  
		    glPopMatrix();


		    glPushMatrix();
		    glLineWidth(2.0f);
		    glColor3f(0.0f, 0.0f, 0.0f);
		    glTranslatef(2.8f, -3.40f, -8.0f);
		    drawBall(0.18f,1);
		    glColor3f(1.0f, 0.0f, 0.0f);
		    drawBall(0.17f,0);
		    glPopMatrix();
		    glPushMatrix();
		    glTranslatef(0.0f, 0.0f, 0.0f);
		    glColor3f(1.0f, 0.0f, 0.0f);
			 drawBall(0.75f,1);
		    glPopMatrix();
		    
		    //lines
		    glBegin(GL_LINES);
		    glColor3f(0.0f, 0.0f, 0.0f);
		    glVertex3f(-1.4f, 1.6f, 0.0f);
		    glVertex3f(1.4f, 1.6f, 0.0f);
		    glVertex3f(-1.4f, -1.6f, 0.0f);
		    glVertex3f(1.4f, -1.6f, 0.0f);
		    glVertex3f(-1.6f, -1.4f, 0.0f);
		    glVertex3f(-1.6f, 1.4f, 0.0f);
		    glVertex3f(1.6f, -1.4f, 0.0f);
		    glVertex3f(1.6f, 1.4f, 0.0f);

		    glVertex3f(-1.4f, 1.8f, 0.0f);
		    glVertex3f(1.4f, 1.8f, 0.0f);
		    glVertex3f(-1.4f, -1.8f, 0.0f);
		    glVertex3f(1.4f, -1.8f, 0.0f);
		    glVertex3f(-1.8f, -1.4f, 0.0f);
		    glVertex3f(-1.8f, 1.4f, 0.0f);
		    glVertex3f(1.8f, -1.4f, 0.0f);
		    glVertex3f(1.8f, 1.4f, 0.0f);
		    glEnd();
		    //glLoadIdentity();
	    	 }

	    	 

	};

void power(){


			glBegin(GL_QUADS);
                        //red color to yellow
            glColor3f(1.0f,0.0f,0.0f);
            glVertex2f(3.5f, 2.0f);
            glVertex2f(4.0f, 2.0f);

                        glColor3f(1.0f,1.0f,0.0f);
                        glVertex2f(4.0f, 1.0f);
                        glVertex2f(3.5f, 1.0f);
                        glEnd();

                        glBegin(GL_QUADS);
                        //yellow to green
                        glColor3f(1.0f,1.0f,0.0f);
                        glVertex2f(3.5f, 1.0f);
                        glVertex2f(4.0f, 1.0f);

                        glColor3f(0.0f,1.0f,0.0f);
                        glVertex2f(4.0f, -0.05f);
                        glVertex2f(3.5f, -0.05f);
                        glEnd();

                        glBegin(GL_QUADS);
                        //white bar
                        glColor3f(0.0f,0.0f,0.0f);
                        glVertex2f(3.45f, (2*mag/0.45));
                        glVertex2f(4.05f, (2*mag/0.45));
                        glVertex2f(4.05f, -0.025f+(2*mag/0.45));
                        glVertex2f(3.45f, -0.025f +(2*mag/0.45));
                        
			glEnd();

}
void coinwithcoin(coin &c1, coin &c2)
{
	if(sqrt(pow(c1.centre_x - c2.centre_x,2)+pow(c1.centre_y-c2.centre_y,2)) > 2*c1.rad)
	{	if(sqrt(pow(c1.centre_x + c1.vel_x- c2.centre_x - c2.vel_x,2)+pow(c1.centre_y +c1.vel_y -c2.centre_y-c2.vel_y, 2)) > 2*c1.rad)
		return ;
	}
	pair<float, float> dist = make_pair(c2.centre_x-c1.centre_x, c2.centre_y-c1.centre_y);
	pair<float, float> vel = make_pair(c2.vel_x-c1.vel_x, c2.vel_y-c1.vel_y);
	float var ; 
	if(dist.first == 0){
		var = 0.1f;  
	}
	else var = dist.second/dist.first;
	//out << var << endl ; 
	float part = -2*(vel.first+var*vel.second)/(2*(1+var*var));

	c2.vel_x += part;
	c2.vel_y += var*part;
	c1.vel_x -= part;
	c1.vel_y -= var*part;

}

void coinwithstriker(striker &c1, coin &c2)
{
	if(sqrt(pow(c1.centre_x - c2.centre_x,2)+pow(c1.centre_y-c2.centre_y,2)) > c2.rad+c1.rad){
		if(sqrt(pow(c1.centre_x + c1.vel_x- c2.centre_x - c2.vel_x,2)+pow(c1.centre_y +c1.vel_y -c2.centre_y-c2.vel_y, 2)) > c2.rad+c1.rad)
		return ;
	}
	pair<float, float> dist = make_pair(c2.centre_x-c1.centre_x, c2.centre_y-c1.centre_y);
	pair<float, float> vel = make_pair(c2.vel_x-c1.vel_x, c2.vel_y-c1.vel_y);
	float var ; 
		if(dist.first == 0){
		 var = 0.1f;  
	}
	else  var = dist.second/dist.first;
	float part = -2*(vel.first+var*vel.second)/(2*(1+var*var));

	c2.vel_x += part;
	c2.vel_y += var*part;
	c1.vel_x -= part;
	c1.vel_y -= var*part;
}

	board backdrop(5.0f);
	striker b(0.0f, -1.75f, 0.15f);
	coin coins[9] = {coin(1, 0.50f, 0.0f, 0.1f), coin(1, -0.25f, 0.433f, 0.1f),
	 coin(1, -0.25f, -0.433f, 0.1f), coin(0, -0.50f, 0.0f, 0.1f), coin(0, 0.25f, 0.433f, 0.1f),
	  coin(0, 0.25f, -0.433f, 0.1f), coin(2, 0.0f, 0.0f, 0.1f), coin(1, -0.25f, 0.0f, 0.1f), coin(0, 0.25f, 0.0f, 0.1f)};

void output(float x, float y, float r, float g, float b, void *font, const char *string)
{
  //char *c;
  
  glRasterPos3f(x, y, -8.0f);
  glColor3f( r, g, b);
  int len, i;
  len = strlen(string);
  for (i = 0; i < len; i++) {
    glutBitmapCharacter(font, string[i]);
  }
}


void tostring(char str[], int num)
{
    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}

void checkpot(){

			if(sqrt(pow(b.centre_x - 2.08f,2)+pow(b.centre_y - 2.08f,2)) < 0.18f){
			//coins[i].setexists();
			cscore -=5 ; 
			b.ballback(-1.75f);
			//cscore += coins[i].getscore();
			//cout << cscore << endl; 
		}
		if(sqrt(pow(b.centre_x + 2.08f,2)+pow(b.centre_y - 2.08f,2)) < 0.18f){
			//coins[i].setexists();
			cscore -=5 ;
			b.ballback(-1.75f); 
			//cout << cscore << endl;
		}
		if(sqrt(pow(b.centre_x - 2.08f,2)+pow(b.centre_y + 2.08f,2)) < 0.18f){
			//coins[i].setexists();
			cscore -=5 ;
			b.ballback(-1.75f); 
			//cout << cscore << endl;
		}
		if(sqrt(pow(b.centre_x + 2.08f,2)+pow(b.centre_y + 2.08f,2)) < 0.18f){
			//coins[i].setexists();
			cscore -=5;
			b.ballback(-1.75f);
			//cout << cscore << endl;
		}
		}			
    
   char buffer[10];
   int hold = 0 ;


	int main(int argc, char **argv) {

		gettimeofday(&t1, NULL);
	    // Initialize GLUT
	    glutInit(&argc, argv);
	    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

	    int w = glutGet(GLUT_SCREEN_WIDTH);
	    int h = glutGet(GLUT_SCREEN_HEIGHT);
	    int windowWidth = w ;
	    int windowHeight = h ;
	    //cout << w ;
	    //cout << h ; 

	    glutInitWindowSize(windowWidth, windowHeight);
	    glutInitWindowPosition((w - windowWidth) / 2, (h - windowHeight) / 2);

	    glutCreateWindow("Carrom");  // Setup the window
	    initRendering();

	    // Register callbacks

	    glutDisplayFunc(drawScene);
	    glutIdleFunc(drawScene);
	    glutKeyboardFunc(handleKeypress1);
	    glutSpecialFunc(handleKeypress2);
	    glutReshapeFunc(handleResize);
	    glutMouseFunc(mouse_use);
   		//glutMotionFunc(mouse_use);
	    glutTimerFunc(5, update, 0);
	    glutMainLoop();
	    return 0;
	}

	// Function to draw objects on the screen
	void drawScene() {
	    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	    glMatrixMode(GL_MODELVIEW);
	    glutFullScreen();

	    //glLoadIdentity();
	    


	 /*   glPushMatrix();
	    glTranslatef(0.3,0,-1.0f);
	    glColor3f(1.0f,0,0);
	    drawBall(0.01f,0);
	    glPopMatrix();*/
	   glLoadIdentity();
	    glPushMatrix();
	    // Draw Box
	    backdrop.draw_layout();
	    
	    // Draw Striker
		glPushMatrix();
		b.coinmake();
		b.linemake();
		glPopMatrix();
		glPushMatrix();
		power();
		glPopMatrix();

		//Draw Coins
		for(int i=0;i<9;i++)
		{	if(coins[i].getexists() == 0){
			glPushMatrix();
			coins[i].coinmake();
			glPopMatrix();
			}
			else if(coins[i].getexists() == 1 && coins[i].added == 0){
				if(coins[i].gettype() == 0 || coins[i].gettype() == 2){
				cscore += coins[i].getscore() ;
				coins[i].added = 1;
				}
				if(coins[i].gettype() == 1){
				cscore -=  5;
				coins[i].added = 1;
				}
				//cout << cscore << endl ;
			}
		}
		
		glPopMatrix();
		//tostring(buffer, cscore);
		//backdrop.remaining();

	//	glLoadIdentity();
		sprintf(buffer, "%d", cscore);
		output(-4.0f,2.3f,0.0f,0.0f,0.0f,GLUT_BITMAP_TIMES_ROMAN_24,"Player Score:");
		output(-4.0f,2.0f,1.0f,0.0f,0.0f,GLUT_BITMAP_TIMES_ROMAN_24,buffer);

	  /*  if (mouseleftdown)
	   { 
      // Convert mouse position to OpenGL's coordinate system
      	float oglx = ((float(mousex)-1366/2)/1366)*1.51;
      	float ogly = (-(float(mousey) - 768/2)/768)*0.83;
      	cout << mousex << mousey << oglx << ogly << endl ;

      // Draw the box
      glColor3f(0.0f, 0.0f, 0.0f);
      glPointSize(12.0f);
      glBegin(GL_POINTS);
         glVertex3f(oglx, ogly,-1.0f);
      glEnd();
	   }*/
		//glColor3f(1.0f,0,0);
		///glRasterPos2f(0,0);
		//glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24,'d');
	    glutSwapBuffers();
	}

	void update(int value) {

	timeval t2 ;
	gettimeofday(&t2, NULL);
	cscore -= t2.tv_sec-t1.tv_sec ; 
	t1 = t2;
	hold=0;
    
	for(int i=0;i<9;i++)
	{	if(coins[i].getexists() == 0)
			coinwithstriker(b, coins[i]);
	}
	for(int i=0;i<9;i++){
		for(int j=i+1;j<9;j++){
			if(coins[i].getexists() == 0 && coins[j].getexists() == 0)
			coinwithcoin(coins[i], coins[j]);

		}
	}
	
	//b.pos_update(box_len*0.91f);
		b.pos_update(4.55f);
	for(int i=0;i<9;i++){
		if(coins[i].getexists() == 0)
			coins[i].pos_update(4.55f);
	}
	for(int i=0;i<9;i++){
		if(sqrt(pow(coins[i].centre_x - 2.08f,2)+pow(coins[i].centre_y - 2.08f,2)) < 0.18f){
			coins[i].setexists();
			//cscore += coins[i].getscore();
			//cout << cscore << endl; 
		}
		if(sqrt(pow(coins[i].centre_x + 2.08f,2)+pow(coins[i].centre_y - 2.08f,2)) < 0.18f){
			coins[i].setexists();
			//cscore += coins[i].getscore();
			//cout << cscore << endl;
		}
		if(sqrt(pow(coins[i].centre_x - 2.08f,2)+pow(coins[i].centre_y + 2.08f,2)) < 0.18f){
			coins[i].setexists();
			//cscore += coins[i].getscore();
			//cout << cscore << endl;
		}
		if(sqrt(pow(coins[i].centre_x + 2.08f,2)+pow(coins[i].centre_y + 2.08f,2)) < 0.18f){
			coins[i].setexists();
			//cscore += coins[i].getscore();
			//cout << cscore << endl;
		}
	}
	checkpot();
	

	if(abs(b.getx())<1e-3 && abs(b.gety())<1e-3)
	{
		hold=1;
	}
	for(int i=0;i<9;i++)
	{
		if((coins[i].getx() >1e-3 || coins[i].gety() > 1e-3 ) && coins[i].getexists() == 0){
			hold = 0 ;
			break;
		}
	}
	if(hold == 1)
		b.ballback(-1.75f);
	
	glutTimerFunc(5, update, 0);
}
	
void drag(int x, int y)
{
	if(!clicked)
		return ;
	float px=((float)x);
	float py=((float)y);
	float a = (py-384.0f)/(768.0f);
	a*=6.62f;
	float ba = (px-683.0f)/(1366.0f);
	ba*=11.12f;
	//cout << ba << endl ;
	
	b.assignpos(ba);	
}	
void mouse_use(int btn,int state,int x,int y)
{
	float px=((float)x);
	float py=((float)y);
	py = (py-384.0f)/(768.0f);
	py*=6.62f;
	px = (px-683.0f)/(1366.0f);
	px*=11.78f;
	float c = b.getcentx();
	//float d = b.getcenty();
	 
	if(btn==GLUT_RIGHT_BUTTON && state==GLUT_DOWN) 
	{		//cout << "px->" << px << "py->" << py << "c->"<< c<< endl;

			if(py>=1.74f && py<=1.91f && px<=(c+0.15f) && px>=(c-0.15f))
			{	cout << "hua" << endl ; 
				clicked = 1;
				glutMotionFunc(drag);
			}
			else clicked = 0;
			cout << clicked << endl ; 
		
	}
	else if(btn==GLUT_LEFT_BUTTON && state==GLUT_UP)
	{
		if(abs(b.getx())>0.0f || abs(b.gety())>0.0f)
			return ;
			hold = 1;
			//cout << "px->" << px << "py->" << py <<  endl;
			if(py<=1.82f && py>=-2.68f && px>=-2.243f && px<=2.243f)
			{
				float magn = sqrt(pow(1.76f-py, 2) + pow(px-c, 2));
				float diffy = 1.76f-py;
				float ang = asin(diffy/magn);
				
				if(px<=c)
					ang = (float)PI - ang;
				magn/=10.97f;
				//b.setang(ang);
				//b.setmag(mag);
				angle = ang ;
				mag =magn ; 
				b.impulse();
			}
		
	}
}


	void initRendering() {

	    glEnable(GL_DEPTH_TEST);        // Enable objects to be drawn ahead/behind one another
	    glEnable(GL_COLOR_MATERIAL);    // Enable coloring
	    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);   // Setting a background color
	}

	// Function called when the window is resized
	void handleResize(int w, int h) {

	    glViewport(0, 0, w, h);
	    glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	    gluPerspective(45.0f, (float)w / (float)h, 0.1f, 200.0f);
	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();
	}
/*
	void handleMouseclick(int button, int state, int x, int y) {

    if (state == GLUT_DOWN)
    {
        if (button == GLUT_LEFT_BUTTON)
            theta += 15;
        else if (button == GLUT_RIGHT_BUTTON)
            theta -= 15;
    }
}
*/
	void handleKeypress1(unsigned char key, int x, int y) {

	    if(key == 27)
	        exit(0);     // escape key is pressed
		else if(key == 32)
		{
			if(abs(b.getx())>0 || abs(b.gety())>0)
				return ;
			b.impulse();
		}
		else if(key == 97){
			angle = (angle+5*PI/180);
			if(angle >= PI)
				angle =0.0f;
		}
		else if(key == 100){
			angle = (angle-(5*PI)/180);
			if(angle <= 0 )
				angle =0;
			//cout << angle <<endl ; 
			} 
	}

	void handleKeypress2(int key, int x, int y) 
	{
		
		if (key == GLUT_KEY_LEFT)
			b.shit_pos(-0.051f);
		if (key == GLUT_KEY_RIGHT)
			b.shit_pos(0.051f);
		if (key == GLUT_KEY_UP)
			if(mag<= 0.44f){
				mag += 0.01f;
			}
		if (key == GLUT_KEY_DOWN)
			if(mag>=0.01f){
				mag -= 0.01f;	
	}

	}

	/*void striker_move(int x,int y){

		float sx = (float)x;
		float px = (sx-left_coor);
		if(px>0 && px < diff bw){
			float screenx = ((px)/diff)*(distance in float)+left coordinate in float;
			b.changepos();
		}
	

	}	
*/


#ifndef RECORDERMANAGER_H
#define RECORDERMANAGER_H

#include "ofMain.h"
#include "ofxXmlSettings.h"

#include "guiManager.h"
#include "boneManager.h"
#include "chromaKeyer.h"

/*
Manages the recording, keying and bone placement.

boneManager : contains a vector of all skeletons saved and master set
cKeyer      : handles keying of frames
gui         : handles the interface and the timeline. each recorded frame of timeline is saved as a button in the gui

saving and loading:
- all files are saved in a directory in bin->data->recordings
- if no directory is set in the text box, a really long one based on the time is created
- to load recordings, enter the directory name in the text box

interface notes
- right click on timeline to turn a frame off

*/


class recorderManager
{
    public:
        recorderManager();
        virtual ~recorderManager();

        // clears timeline and skeleton keyframes
        void reset();

        void setup( int w, int h );

        // updates with data from left + right camera
        void update( unsigned char * pixelsL, unsigned char * pixelsR );

        void draw();

        // output for osc

        // updates all vectors so we have current info on bones
        void    computeRotation();

        Matrix3f getNeckRotation();
        Matrix3f getTorsoRotation();
        Matrix3f getPelvisRotation();

        Matrix3f getLeftShoulderRotation();
        Matrix3f getLeftElbowRotation();

        Matrix3f getRightShoulderRotation();
        Matrix3f getRightElbowRotation();

        Matrix3f getLeftHipRotation();
        Matrix3f getLeftKneeRotation();

        Matrix3f getRightHipRotation();
        Matrix3f getRightKneeRotation();


        // saves current pixels to image vector and adds a new frame to timeline
        void addFrame(unsigned char * pixelsL, unsigned char * pixelsR);
        // creates small thumbnails for display on timeline
        void makeThumbnails();


        // creates directory and saves images + session data ( on/off frames, fps )
        void save();
        // saves the frames
        void saveFrames();
        // save sesion data to xml
        void saveSessionData(string path);
        // saves skeleton data for each keyframe to xml
        void saveSkeletonData(string path);


        void clearFrames();
        void activateFrame( int id );
        void deactivateFrame( int id );

        // drawing
        void drawTimeline();
        void drawInOutPoints();

        // interface
        void mousePressed(int x, int y, int button);
        bool mouseDragged(int x, int y, int button);
        void mouseMoved(int x, int y);
        void mouseReleased(int x, int y, int button);

        // for entering directory name
        bool isTypingFolder(){ return gui.folderNameButton.bOn; }
        void enterFolderName( char key );

        void toggleRecording();
        void toggleRecording(bool bOn);
        void togglePaused();
        void togglePaused(bool bOn);

        // finds next "on" frame in timeline
        int  getNextFrame();
        bool isRecording(){ return bRecording; };

        // returns total number of frames (on and off)
        int  numFrames(){   return framesL.size(); };

        // returns current directory
        string getOutputDirectory(){ return outputDir; }

        // sets current frame
        void setFrameOn( int pos ){ frameOn = pos; frameOn = MIN(numFrames()-1,MAX(0,frameOn)); };

        // swaps camera views in recorded frames
        void swapCameras();

        //----- gui
        guiManager  gui;
        int lastX, lastY;

        //---- manages the skeleton
        boneManager     boneMan;

        //----- manages chroma keying
        chromaKeyer     cKeyer;             // object to key images

        //--- fps
        float   fpsRec;                     // framerate for recording
        float   fpsPlay;                    // framerate for playback

                //----- in and out points
        void setInPos();
        void setOutPos();
        int inPos;
        int outPos;

    protected:



        //----- frames
        vector<ofImage> framesR;            // recorded frames from right camera
        vector<ofImage> framesL;            // recorded frames from left camera

        vector<ofImage> thumbL;             // thumbnails for display on the timeline
        vector<ofImage> thumbR;




        //----- keying
        ofxCvColorImage keySrc, keyDst;     // images used to pass frames to the keyer
        bool            bKeying;            // toggle keying on and off

        //----- timers
        float timer, startTime;             // timers for counting the framerate
        int   frameOn;                      // current frame displayed - either being recorded, selected or playing


        //----- settings
        bool    bRecording;                 // starts and stops recording
        bool    bPaused;                    // paused playback
        int     vidW, vidH;                 // width and height of video input (camera or movie file)


        //----- display
        ofTrueTypeFont  font;

        //----- output
        int     recordCounter;              // counter for saving multiple sets of images
        string  outputDir;                  // name of current directory to save into
        bool    bTypingName;                // on while text box is selected

        //---- interface stuff
        bool bShiftMouseOn;


    private:
};

#endif // RECORDERMANAGER_H

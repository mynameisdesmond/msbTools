#include "testApp.h"

#ifdef TARGET_WIN32

	#include <windows.h>
	#include <stdio.h>
	#include <conio.h>
	#include <tchar.h>

	TCHAR szName[]=TEXT("Global\\MyFileMappingObject");
#else

	#include <sys/mman.h>

#endif

#include "actor.h"
#include "sliderButton.h"
#include "textInputButton.h"
#include "assignButton.h"
#include "msbLight.h"

#define BUF_SIZE 640*480*4*32


//--------------------------------------------------------------
void testApp::setup(){

    bFullscreen=false;
    cutOffDepth=4096;
    bSetCutoffToZero=false;
    bShareMemory=false;
    bSendSkeleton=false;

    skelNo = 1;
    channel=1;
    ipAddress="127.0.0.1";

    osc_sender.setup(ipAddress,31840+channel);

	//kinect.bImage=true;

    msbSetup();

    //interface setup
    interfaceSetup();

    //OF_STUFF

	//kinect.init(true);  //shows infrared image
	kinect.init();
	kinect.setVerbose(true);
	kinect.open();

    filemappingSetup();

}

void testApp::msbSetup(){

    //MSB setup
    renderer=Renderer::getInstance();
    input=Input::getInstance();

    renderer->loadPreferences();

    renderer->setup();

    renderer->camActor=new Actor;
    renderer->camActor->setLocation(Vector3f(0,0,-5));
    renderer->camActor->postLoad();

    registerProperties();

    if (!kinect.bImage)
        return;

    //Adding MSB content
    //heightfield based on videoTexture from OF
    Actor* myActor = new Actor;
    myActor->setLocation(Vector3f(-0.5,-1.0,-5));
    myActor->setRotation(Vector3f(0,180,0));
    myActor->color=Vector4f(1,1,1,1);
    myActor->drawType=DRAW_POINTPATCH;
    myActor->particleScale=250;
    myActor->scale=Vector3f(1,-1,1);
    myActor->bTextured=true;
    myActor->textureID="NULL";
    myActor->sceneShaderID="heightfield";
    myActor->setup();

    renderer->actorList.push_back(myActor);
    renderer->layerList[0]->actorList.push_back(myActor);

    patchActor=myActor;
}

void testApp::interfaceSetup(){


    SliderButton *but;

    but= new SliderButton;
    but->location.x=10;
    but->location.y=740;
    but->scale.x=1000;
    but->color=Vector4f(0.3,0.3,0.3,1.0);
    but->scale.y=10;
    but->name="RotateViewX";
    but->tooltip="drag to Change";
    but->setLocation(but->location);
    but->textureID="icon_flat";
    but->setup();
    but->bDrawName=true;
    but->parent=this;
    but->bVertical=false;
    but->sliderValue=0.5;
    renderer->buttonList.push_back(but);

    TextInputButton *tiBut;

    tiBut= new TextInputButton;
    tiBut->location.x=10;
    tiBut->location.y=550;
    tiBut->scale.x=100;
    tiBut->scale.y=12;
    tiBut->color=Vector4f(0.5,0.5,0.5,1.0);
    tiBut->textureID="icon_flat";
    tiBut->name="cutOffDepth";
    tiBut->bDrawName=true;
    tiBut->setLocation(tiBut->location);
    tiBut->parent=this;
    tiBut->buttonProperty="CUTOFFDEPTH";
    renderer->buttonList.push_back(tiBut);
    tiBut->bPermanent=true;
    tiBut->setup();

    tiBut= new TextInputButton;
    tiBut->location.x=10;
    tiBut->location.y=570;
    tiBut->scale.x=100;
    tiBut->scale.y=12;
    tiBut->color=Vector4f(0.5,0.5,0.5,1.0);
    tiBut->textureID="icon_flat";
    tiBut->name="previewRes";
    tiBut->bDrawName=true;
    tiBut->setLocation(tiBut->location);
    tiBut->parent=patchActor;
    tiBut->buttonProperty="PARTICLESCALE";
    renderer->buttonList.push_back(tiBut);
    tiBut->bPermanent=true;
    tiBut->setup();

    tiBut= new TextInputButton;
    tiBut->location.x=10;
    tiBut->location.y=600;
    tiBut->scale.x=100;
    tiBut->scale.y=12;
    tiBut->color=Vector4f(0.5,0.5,0.5,1.0);
    tiBut->textureID="icon_flat";
    tiBut->name="setCutoffZero";
    tiBut->bDrawName=true;
    tiBut->setLocation(tiBut->location);
    tiBut->parent=this;
    tiBut->buttonProperty="BSETCUTOFFTOZERO";
    renderer->buttonList.push_back(tiBut);
    tiBut->bPermanent=true;
    tiBut->setup();

    tiBut= new TextInputButton;
    tiBut->location.x=10;
    tiBut->location.y=620;
    tiBut->scale.x=100;
    tiBut->scale.y=12;
    tiBut->color=Vector4f(0.5,0.5,0.5,1.0);
    tiBut->textureID="icon_flat";
    tiBut->name="send to MSB";
    tiBut->bDrawName=true;
    tiBut->setLocation(tiBut->location);
    tiBut->parent=this;
    tiBut->buttonProperty="BSHAREMEMORY";
    renderer->buttonList.push_back(tiBut);
    tiBut->bPermanent=true;
    tiBut->setup();

    tiBut= new TextInputButton;
    tiBut->location.x=10;
    tiBut->location.y=640;
    tiBut->scale.x=100;
    tiBut->scale.y=12;
    tiBut->color=Vector4f(0.5,0.5,0.5,1.0);
    tiBut->textureID="icon_flat";
    tiBut->name="send Skeleton";
    tiBut->bDrawName=true;
    tiBut->setLocation(tiBut->location);
    tiBut->parent=this;
    tiBut->buttonProperty="BSENDSKELETON";
    renderer->buttonList.push_back(tiBut);
    tiBut->bPermanent=true;
    tiBut->setup();

    tiBut= new TextInputButton;
    tiBut->location.x=10;
    tiBut->location.y=660;
    tiBut->scale.x=100;
    tiBut->scale.y=12;
    tiBut->color=Vector4f(0.5,0.5,0.5,1.0);
    tiBut->textureID="icon_flat";
    tiBut->name="Skel Number";
    tiBut->bDrawName=true;
    tiBut->setLocation(tiBut->location);
    tiBut->parent=this;
    tiBut->buttonProperty="SKELNO";
    renderer->buttonList.push_back(tiBut);
    tiBut->bPermanent=true;
    tiBut->setup();


}

void testApp::filemappingSetup(){


#ifdef TARGET_WIN32

   hMapFile = CreateFileMapping(
                 INVALID_HANDLE_VALUE,    // use paging file
                 NULL,                    // default security
                 PAGE_READWRITE,          // read/write access
                 0,                       // maximum object size (high-order DWORD)
                 BUF_SIZE,                // maximum object size (low-order DWORD)
                 szName);                 // name of mapping object

    if (hMapFile == NULL)
    {
      _tprintf(TEXT("Could not create file mapping object (%d).\n"),
             GetLastError());
      //OF_EXIT_APP(1);
    }
    pBuf = (LPTSTR) MapViewOfFile(hMapFile,   // handle to map object
                        FILE_MAP_ALL_ACCESS, // read/write permission
                        0,
                        0,
                        BUF_SIZE);


   if (pBuf == NULL)
   {
      _tprintf(TEXT("Could not map view of file (%d).\n"),
             GetLastError());

       CloseHandle(hMapFile);

      //OF_EXIT_APP(0);
   }

#else
	sourcebuffer=NULL;
	//taken from: http://mike.kronenberg.org/?p=40
	//all credit goes to Mike Kronenberg!

	// we need a dummy file, this gives us the possibility to
	// access the shared memory over the filesystem
	sharedMemFile = malloc(BUF_SIZE);

	// open trunkate/create read/write for everybody
	fd = open("/tmp/msbRamFile", O_CREAT|O_RDWR, 0666);
	if (fd == -1) {
		cout <<"Could not open '/tmp/msbRamFile'"<<endl;
	}
	ret = write(fd, sharedMemFile, BUF_SIZE); // create the file
	ret = close(fd);
	free(sharedMemFile);

	//attach shared memory file
	fd = open("/tmp/msbRamFile", O_RDWR);
	if(!fd)
		cout << "Could not open '/tmp/msbRamFile'"<<endl;

	// load the file into memory, shared, read & write access
	sourcebuffer = mmap( 0, BUF_SIZE, PROT_READ|PROT_WRITE, MAP_FILE|MAP_SHARED, fd, 0);
	if(!sourcebuffer)
		cout << "Could not mmap '%s.vga'"<< endl;

	// once the file is mapped, we can dispose of the filehandle
	close(fd);

#endif
    myPic= new float[640*480*4];

}

void testApp::registerProperties(){

   createMemberID("BSHAREMEMORY",&bShareMemory,this);
   createMemberID("CUTOFFDEPTH",&cutOffDepth,this);
   createMemberID("BSETCUTOFFTOZERO",&bSetCutoffToZero,this);
   createMemberID("BSENDSKELETON",&bSendSkeleton,this);
   createMemberID("SKELNO",&skelNo,this);
}

int testApp::shareMemory(){


    //store xyz values in rgb pixels, soon.
    if (kinect.getDepthPixels()){


        //construct full color image
        for (int i=0;i<640*480*4;i+=4){
            myPic[i]=(float)kinect.getPixels()[640*480 * 3 - (i/4) *3] * 1.0f/255.0f;
            myPic[i+1]=(float)kinect.getPixels()[640*480 * 3 - (i/4) *3 + 1] * 1.0f/255.0f;
            myPic[i+2]=(float)kinect.getPixels()[640*480 * 3 - (i/4) *3 + 2] * 1.0f/255.0f;
            //alpha from here
            myPic[i+3]=(float)kinect.getDistancePixels()[640*480 - i/4];
            if (myPic[i+3]>cutOffDepth){
                if (bSetCutoffToZero)
                    myPic[i+3]=0.0f;
                else
                    myPic[i+3]=1.0f;
            }else{
                myPic[i+3]=myPic[i+3]/cutOffDepth;
                if (myPic[i+3]<=0){
                    if (bSetCutoffToZero)
                        myPic[i+3]=0.0f;
                    else
                        myPic[i+3]=1.0f;
                }

            }

        }
#ifdef TARGET_WIN32
       CopyMemory((PVOID)pBuf, myPic, (640*480 * 4* sizeof(float)));
#else
		memcpy(sourcebuffer, myPic, (640*480 * 4* sizeof(float)));
#endif
	}
   // _getch();



}

//--------------------------------------------------------------
void testApp::update(){

	ofBackground(100, 100, 100);
	kinect.update();

    if (bShareMemory)
        shareMemory();

    if (bSendSkeleton){
        for (int i=0;i<kinect.users.size();i++){
            if (kinect.users[i]->bCalibrated)
                sendSkeleton(i);
        }
    }

    renderer->update();

    if (kinect.bImage)
        patchActor->ofTexturePtr=&kinect.getDepthTextureReference();

}


void testApp::sendSkeleton(int i){

        if (i!=skelNo)
            return;

        XnSkeletonJointOrientation joint[15];
        int j=0;
        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(kinect.users[i]->userID, XN_SKEL_HEAD, joint[j++]);                    //0
        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(kinect.users[i]->userID, XN_SKEL_NECK, joint[j++]);                    //1

        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(kinect.users[i]->userID, XN_SKEL_LEFT_SHOULDER, joint[j++]);           //2
        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(kinect.users[i]->userID, XN_SKEL_LEFT_ELBOW, joint[j++]);              //3
        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(kinect.users[i]->userID, XN_SKEL_LEFT_HAND, joint[j++]);               //4

        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(kinect.users[i]->userID, XN_SKEL_RIGHT_SHOULDER, joint[j++]);         //5
        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(kinect.users[i]->userID, XN_SKEL_RIGHT_ELBOW, joint[j++]);              //6
        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(kinect.users[i]->userID, XN_SKEL_RIGHT_HAND, joint[j++]);              //7

        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(kinect.users[i]->userID, XN_SKEL_TORSO, joint[j++]);                   //8
        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(kinect.users[i]->userID, XN_SKEL_LEFT_HIP, joint[j++]);                //9
        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(kinect.users[i]->userID, XN_SKEL_RIGHT_HIP, joint[j++]);               //10
        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(kinect.users[i]->userID, XN_SKEL_LEFT_KNEE, joint[j++]);               //11
        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(kinect.users[i]->userID, XN_SKEL_LEFT_FOOT, joint[j++]);               //12
        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(kinect.users[i]->userID, XN_SKEL_RIGHT_KNEE, joint[j++]);              //13
        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(kinect.users[i]->userID, XN_SKEL_RIGHT_FOOT, joint[j++]);              //14

        XnSkeletonJointPosition body;
        kinect.userGenerator.GetSkeletonCap().GetSkeletonJointPosition(kinect.users[i]->userID, XN_SKEL_TORSO, body);



        Matrix4f boneMat[15];



        Matrix4f nullMat;


        ofxOscMessage myMessage;

        string oscAddress = "/pilot";


            //torso as root
            boneMat[8]=makeMatrix4(&joint[8]);

            //neck
            boneMat[1]=boneMat[8].inverse() * makeMatrix4(&joint[1]);


            //right shoulder - multiply with an additional rotation of 90 degrees around x-axis
            boneMat[2]=(boneMat[8]).inverse() * makeMatrix4(&joint[2]);
            //right elbow
            boneMat[3]= (boneMat[8] * boneMat[2] ).inverse() *  makeMatrix4(&joint[3]);



            //left shoulder
            boneMat[5]=(boneMat[8]).inverse() * makeMatrix4(&joint[5]);
            //left elbow
            boneMat[6]= (boneMat[8] * boneMat[5] ).inverse() * makeMatrix4(&joint[6]);





            //left hip
            boneMat[9]= makeMatrix4(&joint[9]);
            //right hip
            boneMat[10]= makeMatrix4(&joint[10]);

            //left knee
            boneMat[11]= (boneMat[9] ).inverse() * makeMatrix4(&joint[11]);

            //right knee
            boneMat[13]= (boneMat[10] ).inverse() * makeMatrix4(&joint[13]);




        for (int i=0;i<15;i++){
            oscAddress += makeMatrixMessage(&myMessage,boneMat[i]);
        }


        oscAddress+="/vector3f";

        myMessage.addFloatArg(body.position.X/250.0);
        myMessage.addFloatArg(body.position.Y/250.0);
        myMessage.addFloatArg(body.position.Z/250.0);

        myMessage.setAddress(oscAddress);

        osc_sender.sendMessage(myMessage);



        //myMatrix.setRotation(myRot);
        //myMatrix.setTranslation(myPos/100.0);
        //for (int i=0;i<16;i++)
//        myMessage.addFloatArg( myMatrix.data[i]);


}


Matrix4f testApp::makeMatrix4(XnSkeletonJointOrientation* joint){

    Matrix3f myMat3;

    for (int i=0;i<9;i++)
        myMat3.data[i]=joint->orientation.elements[i];

    myMat3 = myMat3.transpose();



    Matrix4f myMat4;

    myMat4.setRotation(myMat3);

    //invert myMat z positions (because they are negative towards sensor...

    myMat4[8]*=-1;
    myMat4[9]*=-1;
    myMat4[10]*=-1;

   // return ( base.inverse() * myMat4 );
    return ( myMat4 );

}

string testApp::makeMatrixMessage( ofxOscMessage* myMessage, Matrix4f myMat4 ){


    for (int i=0;i<16;i++){
         myMessage->addFloatArg( myMat4.data[i]);
    }

    return "/matrix4f";
}

//--------------------------------------------------------------
void testApp::draw(){

    if (bFullscreen){

        kinect.drawDepth(0, 0, renderer->screenX, renderer->screenY);

    }else{


        glEnable(GL_LIGHTING);
        renderer->draw();
        glDisable(GL_LIGHTING);

        ofSetColor(255, 255, 255);

        kinect.drawDepth(10, 50, 400, 300);
        kinect.draw(420, 50, 400, 300);
    }


}

//--------------------------------------------------------------
void testApp::exit(){

#ifdef TARGET_WIN32
    UnmapViewOfFile((void*)pBuf);
    CloseHandle(hMapFile);
#else
	//... do something intresting with the shared memory, then unmap
	if (sourcebuffer)
		munmap(sourcebuffer, BUF_SIZE);
#endif


}

//--------------------------------------------------------------
void testApp::keyPressed (int key){

    input->normalKeyDown(key,mouseX,mouseY);
    input->specialKeyDown(key,mouseX,mouseY);

    if (key=='f')
        bFullscreen=!bFullscreen;

}

void testApp::keyReleased(int key){

    input->keyUp(key,mouseX,mouseY);
    input->specialKeyUp(key,mouseX,mouseY);

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

    input->moveMouse(x,y);

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

    input->dragMouse(x,y);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

    input->pressedMouse(button,0,x,y);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

    input->pressedMouse(button,1,x,y);
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{}

void testApp::trigger(Actor* other){

    if (other->name=="RotateViewX"){
        float slValue= 0.0f;
        slValue= ((SliderButton*)other)->sliderValue;
        patchActor->setRotation(Vector3f(0,slValue * 360.0,0));
    }
    if (other->name=="cutOffDepth")
        kinect.cutOffFar=cutOffDepth;
}

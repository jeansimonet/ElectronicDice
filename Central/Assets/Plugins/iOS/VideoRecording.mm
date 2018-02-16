//
//  VideoRecording.mm
//  VideoRecordingPlugin
//
//  Created by Mayank Gupta on 15/01/15.
//  Copyright (c) 2015 Mayank Gupta. All rights reserved.
//


#import <Foundation/Foundation.h>
#import "VideoRecording.h"
#import "UnityAppController.h"


@interface VideoRecording (){
    UnityAppController *objectUnityAppController;
}

@end

@implementation VideoRecording
#pragma mark Unity bridge

+ (VideoRecording *)pluginSharedInstance {
    static VideoRecording *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[VideoRecording alloc] init];
        
    });
    return sharedInstance;
}

#pragma mark Cleanup


#pragma mark Ios Methods
//-------------------------------------------------------------------------------------------------
-(bool)recodingViewShow :(NSString *)modeType
                        :(NSString *)origin_x
                        :(NSString *)origin_y
                        :(NSString *)width
                        :(NSString *)height
                        :(bool) isShowRecording{
    if(videoController == nil){
        CGRect defaultFrame = CGRectMake(10,100,300,360);
        videoController = [[MPMoviePlayerController alloc] init];
        [videoController.view setFrame:defaultFrame];
    }
    NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
    numberFormatter.numberStyle = NSNumberFormatterDecimalStyle;
    videoController.view.frame = CGRectMake([numberFormatter numberFromString:origin_x].floatValue,
                                            [numberFormatter numberFromString:origin_y].floatValue,
                                            [numberFormatter numberFromString:width].floatValue,
                                            [numberFormatter numberFromString:height].floatValue);
    objectUnityAppController = GetAppController();
    if(objectUnityAppController.rootView == nil)
        return false;
    else{
        [objectUnityAppController.rootView addSubview:videoController.view];
        videoController.view.hidden = !isShowRecording;
        return true;
    }
    return true;
}
//-------------------------------------------------------------------------------------------------
-(void)takeRecordingFromCamera{
    if ([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypeCamera]) {
        UIImagePickerController *picker = [[UIImagePickerController alloc] init];
        picker.delegate = self;
        picker.allowsEditing = YES;
        picker.sourceType =  UIImagePickerControllerSourceTypeCamera;
        picker.mediaTypes = [[NSArray alloc] initWithObjects: (NSString *) kUTTypeMovie, nil];
        if(objectUnityAppController ==nil)
            objectUnityAppController = GetAppController();
        [objectUnityAppController.rootViewController presentViewController:picker animated:YES completion:NULL];
        
    }
    else
    {
        ;
    }
    
}
//-------------------------------------------------------------------------------------------------
- (void)selectfromPhotoLibrary{
    if ([UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypePhotoLibrary]) {
        
        UIImagePickerController *picker = [[UIImagePickerController alloc] init];
        picker.delegate = self;
        picker.allowsEditing = YES;
        picker.sourceType = UIImagePickerControllerSourceTypePhotoLibrary;
        picker.mediaTypes = [[NSArray alloc] initWithObjects: (NSString *) kUTTypeMovie, nil];
        if(objectUnityAppController ==nil)
            objectUnityAppController = GetAppController();
        [objectUnityAppController.rootViewController presentViewController:picker animated:YES completion:NULL];
    }
}
//-------------------------------------------------------------------------------------------------
-(void)showRecordingView : (bool) isShowRecording{
    videoController.view.hidden = !isShowRecording;
    if(isShowRecording == TRUE)
        [videoController play];
    else
        [videoController stop];
}
//-------------------------------------------------------------------------------------------------
-(void)toSaveRecordingToCameraRoll:(NSString *)msgReceivingGameObjectName
                                  :(NSString *)msgReceivingMethodtName{
    ALAssetsLibrary *library = [[ALAssetsLibrary alloc] init];
    [library writeVideoAtPathToSavedPhotosAlbum:videoURL completionBlock:^(NSURL *assetURL, NSError *error){
        if (error) {
            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Error" message:@"Video Saving Failed"
                                                           delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [alert show];
        } else {
            NSString* recordingStatus = @"Recording Saved To CameraRoll";
            const char *msgImageSaved = [recordingStatus cStringUsingEncoding:NSASCIIStringEncoding];
            const char *gameObjectName = [msgReceivingGameObjectName cStringUsingEncoding:NSASCIIStringEncoding];
            const char *methodName = [msgReceivingMethodtName cStringUsingEncoding:NSASCIIStringEncoding];
            UnitySendMessage(gameObjectName,methodName, msgImageSaved);
            
            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Video Saved" message:@"Saved To Album"
                                                           delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [alert show];
        }
    }];
}
//-------------------------------------------------------------------------------------------------
-(void)toSaveRecordingToSpecificAlbum:(NSString *)msgReceivingGameObjectName
                                     :(NSString *)msgReceivingMethodtName
                                     :(NSString *)albumName{
    ALAssetsLibrary *library = [[ALAssetsLibrary alloc] init];
    [library saveVideo:videoURL toAlbum:albumName completion:^(NSURL *assetURL, NSError *error){
        if (error) {
            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Error" message:@"Video Saving Failed"
                                                           delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [alert show];
        } else {
            NSString* recordingStatus = @"Recording Saved To Album";
            const char *msgImageSaved = [recordingStatus cStringUsingEncoding:NSASCIIStringEncoding];
            const char *gameObjectName = [msgReceivingGameObjectName cStringUsingEncoding:NSASCIIStringEncoding];
            const char *methodName = [msgReceivingMethodtName cStringUsingEncoding:NSASCIIStringEncoding];
            UnitySendMessage(gameObjectName,methodName, msgImageSaved);
            UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Video Saved" message:@"Saved To Album"
                                                           delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
            [alert show];
        }
    }
               failure:nil];
}
//-------------------------------------------------------------------------------------------------
-(void)toLoadRecordingToUnity:(NSString *)msgReceivingGameObjectName
                             :(NSString *)msgReceivingMethodtName{
    NSLog(@"Loading-------Here2");
    const char *msgImageSaved = [moviePath cStringUsingEncoding:NSASCIIStringEncoding];
    const char *gameObjectName = [msgReceivingGameObjectName cStringUsingEncoding:NSASCIIStringEncoding];
    const char *methodName = [msgReceivingMethodtName cStringUsingEncoding:NSASCIIStringEncoding];
    //UnitySendMessage(gameObjectName,methodName, msgImageSaved);
    NSLog(@"Loading-------Here3--%s---%s---%s",msgImageSaved,gameObjectName,methodName);
    UnitySendMessage(gameObjectName,methodName, msgImageSaved);
}
//-------------------------------------------------------------------------------------------------
- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary *)info {
    videoURL = info[UIImagePickerControllerMediaURL];
    [picker dismissViewControllerAnimated:YES completion:NULL];
    [videoController setContentURL:videoURL];
    if(videoController.view.hidden == FALSE)
        [videoController play];
    moviePath = (NSString *)[[info objectForKey:UIImagePickerControllerMediaURL] path];
    
}
//-------------------------------------------------------------------------------------------------
- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker {
    
    [picker dismissViewControllerAnimated:YES completion:NULL];
    
}
//-------------------------------------------------------------------------------------------------

-(void)video:(NSString*)videoPath didFinishSavingWithError:(NSError*)error contextInfo:(void*)contextInfo {
    if (error) {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Error" message:@"Video Saving Failed"
                                                       delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
    } else {
        UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Video Saved" message:@"Saved To Album"
                                                       delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alert show];
    }
}
//------------------------------------------------------------------------------------------------------
@end

// Helper method used to convert NSStrings into C-style strings.
NSString *CreateStr(const char* string) {
    if (string) {
        return [NSString stringWithUTF8String:string];
    } else {
        return [NSString stringWithUTF8String:""];
    }
}


// Unity can only talk directly to C code so use these method calls as wrappers
// into the actual plugin logic.
extern "C" {
    bool _ShowRecordingCapture(const char *modeType,const char *origin_x,const char *origin_y,const char *width,const char *height,bool isShowRecording){
        VideoRecording *objVideoCapture = [VideoRecording pluginSharedInstance];
        return [objVideoCapture recodingViewShow:CreateStr(modeType)
                                                :CreateStr(origin_x)
                                                :CreateStr(origin_y)
                                                :CreateStr(width)
                                                :CreateStr(height)
                                                :isShowRecording];
    }
    //-------------------------------------------------------------------------------------------------
    void _OpenCamera(){
        VideoRecording *objVideoCapture = [VideoRecording pluginSharedInstance];
        [objVideoCapture takeRecordingFromCamera];
    }
    //-------------------------------------------------------------------------------------------------
    void _OpenGallery(){
        VideoRecording *objVideoCapture = [VideoRecording pluginSharedInstance];
        [objVideoCapture selectfromPhotoLibrary];
    }
    //-------------------------------------------------------------------------------------------------
    void _ShowCapturedRecording (bool isShowRecording){
        VideoRecording *objVideoCapture = [VideoRecording pluginSharedInstance];
        [objVideoCapture showRecordingView:isShowRecording];
    }
    //-------------------------------------------------------------------------------------------------
    void _ToSaveRecordingToCameraRoll(const char *msgReceivingGameObjectName,const char *msgReceivingMethodtName){
        VideoRecording *objVideoCapture = [VideoRecording pluginSharedInstance];
        [objVideoCapture toSaveRecordingToCameraRoll:CreateStr(msgReceivingGameObjectName)
                                                    :CreateStr(msgReceivingMethodtName)];
    }
    //-------------------------------------------------------------------------------------------------
    void _ToSaveRecordingToSpecificAlbum(const char *msgReceivingGameObjectName,const char *msgReceivingMethodtName,const char *albumName){
        VideoRecording *objVideoCapture = [VideoRecording pluginSharedInstance];
        [objVideoCapture toSaveRecordingToSpecificAlbum:CreateStr(msgReceivingGameObjectName)
                                                       :CreateStr(msgReceivingMethodtName)
                                                       :CreateStr(albumName)];
    }
    //-------------------------------------------------------------------------------------------------
    void _ToLoadRecordingToUnity(const char *msgReceivingGameObjectName,const char *msgReceivingMethodtName){
        VideoRecording *objVideoCapture = [VideoRecording pluginSharedInstance];
        [objVideoCapture toLoadRecordingToUnity:CreateStr(msgReceivingGameObjectName)
                                               :CreateStr(msgReceivingMethodtName)];
    }
    
    
}

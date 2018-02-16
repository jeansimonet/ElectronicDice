
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "ALAssetsLibrary+CustomPhotoAlbum.h"
#import <MediaPlayer/MediaPlayer.h>
#import <MobileCoreServices/MobileCoreServices.h>
#import <AssetsLibrary/AssetsLibrary.h>

@interface VideoRecording : NSObject<UIImagePickerControllerDelegate, UINavigationControllerDelegate>
{
    MPMoviePlayerController *videoController;
    NSURL *videoURL;
    NSString *moviePath;
}
@end

package kr.co.brgames.cdk;

import android.content.res.AssetFileDescriptor;
import android.graphics.RectF;
import android.media.MediaPlayer;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;

class CSVideoView implements SurfaceHolder.Callback {
    private long _target;
    //private VideoView _videoView;
    private RelativeLayout _layout;
    private RectF _frame;
    private boolean _loading;
    private boolean _scaleToFit;
    SurfaceView _surfaceView;
    SurfaceHolder _surfaceHolder;
    MediaPlayer _mediaPlayer;
    
    private static final String TAG = "CSVideoView";

    public CSVideoView(long target) {
        _target = target;

        _surfaceView = new SurfaceView(CSActivity.sharedActivity());
        _surfaceHolder = _surfaceView.getHolder();
        _surfaceHolder.addCallback(this);

        CSActivity.sharedActivity().queueEvent(() -> {
            /*
            _videoView = new VideoView(CSActivity.sharedActivity());
            // todo : 나중에 추가 할수도.
            MediaController controller=new MediaController(CSActivity.sharedActivity());
            controller.setAnchorView(_videoView);
            controller.setMediaPlayer(_videoView);

            _videoView.setBackgroundColor(android.graphics.Color.TRANSPARENT);


            //동영상 재생이 완료된 걸 알 수 있는 리스너
            _videoView.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
                @Override
                public void onCompletion(MediaPlayer mp) {
                    nativeFinish(_target);
                }
            });

            _videoView.setOnPreparedListener(new MediaPlayer.OnPreparedListener() {
                public void onPrepared(MediaPlayer mp) {
                    _videoView.start();
                }
            });
*/
            _layout = new RelativeLayout(CSActivity.sharedActivity());
            _layout.setBackgroundColor(android.graphics.Color.TRANSPARENT);
            _layout.setLayoutParams(new FrameLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
            _layout.addView(_surfaceView);
        });
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
/**
 * surface 생성
 */
        if (_mediaPlayer == null) {
            _mediaPlayer = new MediaPlayer();
        } else {
            _mediaPlayer.reset();
        }
/*
        try {
// local resource : raw에 포함시켜 놓은 리소스 호출
            //Uri uri = Uri.parse("android.resource://" + CSActivity.sharedActivity().getPackageName() + "/raw/teset.mp4");
            //_mediaPlayer.setDataSource(CSActivity.sharedActivity(), uri);

// external URL : 외부 URL이나 path를 지정한 리소스

            String path = "http://download.atmark-techno.com/sample/bbb/big-buck-bunny-30sec-800x480.mp4";
 //           String path = "test.mp4";
_mediaPlayer.setDataSource(path);

            _mediaPlayer.setDisplay(holder); // 화면 호출
            _mediaPlayer.prepare(); // 비디오 load 준비
            _mediaPlayer.setOnCompletionListener(completionListener); // 비디오 재생 완료 리스너
// mediaPlayer.setOnVideoSizeChangedListener(sizeChangeListener); // 비디오 크기 변경 리스너

        } catch (Exception e) {
            e.printStackTrace();
        }
        */
    }

    MediaPlayer.OnCompletionListener completionListener = new MediaPlayer.OnCompletionListener() {
        /**
         * 비디오가 전부 재생된 상태의 리스너
         * @param mp : 현재 제어하고 있는 MediaPlayer
         */
        @Override
        public void onCompletion(MediaPlayer mp) {
           // playButton.setText("Play");
        }
    };

    MediaPlayer.OnVideoSizeChangedListener sizeChangeListener = new MediaPlayer.OnVideoSizeChangedListener() {
        @Override
        public void onVideoSizeChanged(MediaPlayer mp, int width, int height) {
        }
    };

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {

    }

    private void startOrPause() {
        /*
        if (_mediaPlayer.isPlaying()) {
            mediaPlayer.pause();
            playButton.setText("Play");
        } else {

            playButton.setText("Pause");
        }
        */
    }

    private void stopNprepare() {
        _mediaPlayer.stop();



        try {
// mediaplayer 재생 준비
            _mediaPlayer.prepare();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void addToView() {
        CSActivity.sharedActivity().queueEvent(() -> {
            ViewGroup parent = CSActivity.sharedActivity().viewGroup();

            parent.addView(_layout);
        });
    }

    private void removeFromView(final boolean released) {
        CSActivity.sharedActivity().queueEvent(() -> CSActivity.sharedActivity().viewGroup().removeView(_layout));
    }

    public void removeFromView() {
        removeFromView(false);
    }

    public void release() {
        removeFromView(true);
        _target = 0;
    }

    public void setFrame(RectF frame) {
        _frame = frame;

        CSActivity.sharedActivity().queueEvent(() -> {
            CSView view = CSActivity.sharedActivity().view();
            RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams((int) _frame.width(), (int) _frame.height());
            params.setMargins((int)_frame.left, (int)_frame.top, view.width() - (int)_frame.right, view.height() - (int)_frame.bottom);
            _surfaceView.setLayoutParams(params);
        });
    }
    
    public void setFrame(float x, float y, float width, float height) {
        setFrame(new RectF(x, y, x + width, y + height));
    }

    public void playStart(final String path) {
        CSActivity.sharedActivity().queueEvent(() -> {
            try {
// local resource : raw에 포함시켜 놓은 리소스 호출
                //Uri uri = Uri.parse("android.resource://" + CSActivity.sharedActivity().getPackageName() + "/raw/teset.mp4");
                //_mediaPlayer.setDataSource(CSActivity.sharedActivity(), uri);

// external URL : 외부 URL이나 path를 지정한 리소스


                //String path = "http://download.atmark-techno.com/sample/bbb/big-buck-bunny-30sec-800x480.mp4";
                //           String path = "test.mp4";

                //FileInputStream fis = new FileInputStream(path);
                //FileDescriptor fd = fis.getFD();
                //_mediaPlayer.setDataSource(fd);
                //URL mp4Url = new URL("http://download.atmark-techno.com/sample/bbb/big-buck-bunny-30sec-800x480.mp4");
                AssetFileDescriptor afd = CSActivity.sharedActivity().getAssets().openFd("data/test.mp4");
                _mediaPlayer.setDataSource(afd.getFileDescriptor(),afd.getStartOffset(),afd.getLength());
                //_mediaPlayer.setDataSource(path);

                _mediaPlayer.setDisplay(_surfaceHolder); // 화면 호출
                _mediaPlayer.prepare(); // 비디오 load 준비
                _mediaPlayer.setOnCompletionListener(completionListener); // 비디오 재생 완료 리스너
                _mediaPlayer.start();
// mediaPlayer.setOnVideoSizeChangedListener(sizeChangeListener); // 비디오 크기 변경 리스너

            } catch (Exception e) {
                e.printStackTrace();
            }
            //_mediaPlayer.start();
            //Uri uriPath = Uri.parse(path);
            //_videoView.setVideoURI(uriPath);
            //_videoView.start();
        });
    }

    public static native void nativeFinish(long target);
}

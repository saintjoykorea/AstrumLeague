package kr.co.brgames.cdk.extension;

import android.util.Log;
import android.widget.Toast;

import com.lockincomp.liappagent.LiappAgent;

import kr.co.brgames.cdk.CSActivity;

class CSLiAppImpl implements CSActivity.ActivityRestartListener {
    private static final String TAG = "CSLiApp";

    private boolean _started;

    public CSLiAppImpl() {
        CSActivity.sharedActivity().addActivityRestartListener(this);

        validate();
    }

    public void dispose() {
        CSActivity.sharedActivity().removeActivityRestartListener(this);
    }

    private void validate() {
        int result = _started ? LiappAgent.LA2() : LiappAgent.LA1();

		switch(result) {
			case LiappAgent.LIAPP_SUCCESS:
				_started = true;
				break;
			case LiappAgent.LIAPP_EXCEPTION:
				Log.e(TAG, String.format("detection code:%x(%s)", result, LiappAgent.GetMessage()));
				Toast.makeText(CSActivity.sharedActivity(), LiappAgent.GetMessage(), Toast.LENGTH_LONG).show();
				break;
			default:
				Log.e(TAG, String.format("detection code:%x(%s)", result, LiappAgent.GetMessage()));
				Toast.makeText(CSActivity.sharedActivity(), LiappAgent.GetMessage(), Toast.LENGTH_LONG).show();
				
				try {
					Thread.sleep(4000);
				}
				catch(InterruptedException e) {}
				
				CSActivity.finishShared();
				break;
		}
    }

    @Override
    public void onActivityRestart() {
        validate();
    }
}

public class CSLiApp {
    private static CSLiAppImpl _impl;

    public static void initialize() {
        CSActivity.sharedActivity().queueEvent(() -> _impl = new CSLiAppImpl());
    }
    
    public static void dispose() {
		if (_impl != null) {
			CSActivity.sharedActivity().queueEvent(() -> {
				_impl.dispose();
				_impl = null;
			});
		}
    }
}

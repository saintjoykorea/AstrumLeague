package kr.co.brgames.cdk;

import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.MediaPlayer.OnErrorListener;
import android.util.Log;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.HashMap;
import java.util.Map;

public class CSAudioMediaPlayer implements OnCompletionListener, OnErrorListener {
    private static class Instance {
        public MediaPlayer player;
        public int control;
        public float volume;
        public int category;
        public int priority;
        public boolean single;
        public boolean userPaused;
    }
    private final List<Instance> _instances;
    private final Map<String, Integer> _singleDurations;

    private static final int MaxInstances = 8;
    
    private static final String TAG = "CSAudioMediaPlayer";

    public CSAudioMediaPlayer() {
        _instances = new ArrayList<>(MaxInstances);

        _singleDurations = new HashMap<>();
    }

    public synchronized void dispose() {
        for (Instance instance : _instances) {
            instance.player.setOnCompletionListener(null);
            instance.player.setOnErrorListener(null);
            instance.player.release();
        }
        _instances.clear();
    }

    public synchronized boolean isSinglePlaying() {
        for (Instance instance : _instances) {
            if (instance.single) return true;
        }
        return false;
    }

    public int getSingleDuration(String path) {
        Integer i = _singleDurations.get(path);

        return i != null ? i : 0;
    }

    private synchronized boolean ready(int priority) {
        if (_instances.size() < MaxInstances) {
            return true;
        }
        int index = -1;

        for (int i = 0; i < _instances.size(); i++) {
            Instance instance = _instances.get(i);

            if (instance.priority <= priority) {
                priority = instance.priority - 1;
                index = i;
            }
        }
        if (index != -1) {
            Instance instance = _instances.get(index);
            instance.player.setOnCompletionListener(null);
            instance.player.setOnErrorListener(null);
            instance.player.release();
            _instances.remove(index);
            return true;
        }
        return false;
    }

    public void play(int control, String path, float mainVolume, float volume, boolean loop, int priority, int category, boolean single) {
        if (ready(priority)) {
            MediaPlayer player = new MediaPlayer();

            player.setAudioStreamType(AudioManager.STREAM_MUSIC);

            AssetFileDescriptor afd = null;
            try {
                if (path.startsWith("assets/")) {
                    afd = CSActivity.sharedActivity().getAssets().openFd(path.substring(7));
                    player.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
                    afd.close();
                    afd = null;
                }
                else {
                    player.setDataSource(path);
                }
                player.prepare();

                if (single && !_singleDurations.containsKey(path)) {
                    int duration = player.getDuration();
                    if (duration > 0) _singleDurations.put(path, player.getDuration());
                }

                float realVolume = mainVolume * volume;
                player.setVolume(realVolume, realVolume);
                player.setLooping(loop);
                player.start();
            }
            catch (Exception e) {
                Log.e(TAG, e.getMessage(), e);
                if (afd != null) {
                    try {
                        afd.close();
                    }
                    catch (IOException _e) {
                        Log.e(TAG, _e.getMessage(), _e);
                    }
                }
                player.release();
                return;
            }
            Instance instance = new Instance();
            instance.player = player;
            instance.control = control;
            instance.volume = volume;
            instance.priority = priority;
            instance.category = category;
            instance.single = single;
            synchronized (this) {
                _instances.add(instance);

                instance.player.setOnCompletionListener(this);
                instance.player.setOnErrorListener(this);
            }
        }
    }

    @Override
    public synchronized void onCompletion(MediaPlayer mp) {
        if (!mp.isLooping()) {
            mp.setOnCompletionListener(null);
            mp.setOnErrorListener(null);
            mp.release();
            for (int i = 0; i < _instances.size(); i++) {
                Instance instance = _instances.get(i);
                if (instance.player == mp) {
                    _instances.remove(i);
                    break;
                }
            }
        }
    }

    @Override
    public synchronized boolean onError(MediaPlayer mp, int what, int extra) {
        Log.e(TAG, "error:" + what + "," + extra);

        mp.setOnCompletionListener(null);
        mp.setOnErrorListener(null);
        mp.release();
        for (int i = 0; i < _instances.size(); i++) {
            Instance instance = _instances.get(i);
            if (instance.player == mp) {
                _instances.remove(i);
                break;
            }
        }
        return false;
    }

    public synchronized void stop(int control) {
        int i = 0;
        while (i < _instances.size()) {
            Instance instance = _instances.get(i);
            if (instance.control == control) {
                instance.player.setOnCompletionListener(null);
                instance.player.setOnErrorListener(null);
                instance.player.release();
                _instances.remove(i);
            }
            else {
                i++;
            }
        }
    }

    public synchronized void pause(int control) {
        for (Instance instance : _instances) {
            if (instance.control == control) {
                instance.userPaused = true;
                try {
                    instance.player.pause();
                }
                catch (IllegalStateException e) {
                    Log.e(TAG, e.getMessage(), e);
                }
            }
        }
    }

    public synchronized void resume(int control) {
        for (Instance instance : _instances) {
            if (instance.control == control) {
                instance.userPaused = false;
                try {
                    instance.player.start();
                }
                catch (IllegalStateException e) {
                    Log.e(TAG, e.getMessage(), e);
                }
            }
        }
    }

    public synchronized void setVolume(int category, float volume) {
        for (Instance instance : _instances) {
            if (instance.category == category) {
                float realVolume = volume * instance.volume;
                try {
                    instance.player.setVolume(realVolume, realVolume);
                }
                catch (IllegalStateException e) {
                    Log.e(TAG, e.getMessage(), e);
                }
            }
        }
    }

    public synchronized void autoPause() {
        for (Instance instance : _instances) {
            try {
                instance.player.pause();
            }
            catch (IllegalStateException e) {
                Log.e(TAG, e.getMessage(), e);
            }
        }
    }

    public synchronized void autoResume() {
        for (Instance instance : _instances) {
            if (!instance.userPaused) {
                try {
                    instance.player.start();
                }
                catch (IllegalStateException e) {
                    Log.e(TAG, e.getMessage(), e);
                }
            }
        }
    }
}

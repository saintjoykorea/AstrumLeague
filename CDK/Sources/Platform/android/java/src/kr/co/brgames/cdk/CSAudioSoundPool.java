package kr.co.brgames.cdk;

import android.content.res.AssetFileDescriptor;
import android.media.SoundPool;
import android.util.Log;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

public class CSAudioSoundPool {
    private final SoundPool soundPool;
    private static class Instance {
        public int control;
        public String path;
        public int soundId;
        public int streamId;
        public float volume;
        public boolean loop;
        public int priority;
        public int category;
        public int size;
        public long singleElapsed;
    }
    private final List<Instance> _instances;
    private int _capacity;
    private int _seq;
    private boolean _loaded;

    private static final int MaxStreams = 24;
    private static final int MaxCapacity = 32 * 1024768;
    private static final int MaxFileSize = 1024768;
    private static final int MaxCount = 256;

    private static final String TAG = "CSAudioSoundPool";

    public CSAudioSoundPool() {
        soundPool = new SoundPool.Builder()
                .setMaxStreams(MaxStreams)
                .build();

        soundPool.setOnLoadCompleteListener((soundPool, sampleId, status) -> {
            _loaded = status == 0;
            synchronized(CSAudioSoundPool.this) {
                CSAudioSoundPool.this.notify();
            }
        });
        _instances = new ArrayList<>();

        _seq = 0xffff;
    }

    public void dispose() {
        _instances.clear();
        soundPool.release();
    }

    private boolean ready(int size, int priority) {
        if (_instances.size() < MaxCount && _capacity + size < MaxCapacity) {
            return true;
        }
        while (!_instances.isEmpty()) {
            Instance instance = _instances.get(0);

            if (instance.priority <= priority) {
                if (instance.streamId != 0) {
                    soundPool.stop(instance.streamId);
                }
                soundPool.unload(instance.soundId);
                _capacity -= instance.size;
                _instances.remove(0);
                
                if (_instances.size() < MaxCount && _capacity + size < MaxCapacity) {
                    return true;
                }
            }
            else {
                break;
            }
        }
        return false;
    }

    public boolean isSinglePlaying() {
        long currentTime = System.currentTimeMillis();
        for(Instance instance : _instances) {
            if (currentTime < instance.singleElapsed) return true;
        }
        return false;
    }

    public boolean play(int control, String path, float mainVolume, float volume, boolean loop, int priority, int category, int singleDuration) {
        float realVolume = mainVolume * volume;

        if (--_seq < 0) _seq = 0xffff;
        int realPriority = (priority << 24) | ((int)(realVolume * 255) << 16) | _seq;

        for (int i = 0; i < _instances.size(); i++) {
            Instance otherInstance = _instances.get(i);

            if (otherInstance.path.equals(path)) {
                _instances.remove(i);
                int index = _instances.size();
                while (index > 0 && _instances.get(index - 1).priority > priority) {
                    index--;
                }
                _instances.add(index, otherInstance);

                otherInstance.control = control;
                otherInstance.category = category;
                otherInstance.priority = priority;
                otherInstance.singleElapsed = singleDuration != 0 ? (loop ? Long.MAX_VALUE : System.currentTimeMillis() + singleDuration) : 0;

                if (otherInstance.loop) {
                    if (loop && volume > otherInstance.volume) {
                        soundPool.setVolume(otherInstance.streamId, realVolume, realVolume);
                        otherInstance.volume = volume;
                    }
                    return true;
                }
                otherInstance.streamId = soundPool.play(otherInstance.soundId, realVolume, realVolume, realPriority, loop ? -1 : 0, 1.0f);
                otherInstance.volume = volume;
                otherInstance.loop = loop;
                return true;
            }
        }
        int soundId;

        AssetFileDescriptor afd = null;

        int size;

        if (path.startsWith("assets/")) {
            try {
                afd = CSActivity.sharedActivity().getAssets().openFd(path.substring(7));
                size = (int) afd.getLength();
                if (size >= MaxFileSize) {
                    return false;
                }
                if (!ready(size, priority)) {
                    return true;
                }
                synchronized (this) {
                    soundId = soundPool.load(afd, 1);
                    if (soundId == 0) {
                        return false;
                    }
                    try {
                        wait();
                    }
                    catch (InterruptedException e) {
                        Log.e(TAG, e.getMessage(), e);
                    }
                }
            }
            catch (IOException e) {
                Log.e(TAG, e.getMessage(), e);
                return false;
            }
            finally {
                if (afd != null) {
                    try {
                        afd.close();
                    }
                    catch (IOException _e) {
                        Log.e(TAG, _e.getMessage(), _e);
                    }
                }
            }
        }
        else {    // storage
            File f = new File(path);
            if (!f.exists()) {
                return true;
            }
            size = (int)f.length();
            if (size >= MaxFileSize) {
                return false;
            }
            if (!ready(size, priority)) {
                return true;
            }
            synchronized (this) {
                soundId = soundPool.load(path, 1);
                if (soundId == 0) {
                    return false;
                }
                try {
                    wait();
                }
                catch (InterruptedException e) {
                    Log.e(TAG, e.getMessage(), e);
                }
            }
        }
        if (!_loaded) {
            return false;
        }
        Instance instance = new Instance();
        instance.control = control;
        instance.path = path;
        instance.soundId = soundId;
        instance.streamId = soundPool.play(soundId, realVolume, realVolume, realPriority, loop ? -1 : 0, 1.0f);
        instance.loop = loop;
        instance.volume = volume;
        instance.priority = priority;
        instance.category = category;
        instance.size = size;
        instance.singleElapsed = singleDuration != 0 ? (loop ? Long.MAX_VALUE : System.currentTimeMillis() + singleDuration) : 0;

        int index = _instances.size();
        while (index > 0 && _instances.get(index - 1).priority > priority) {
            index--;
        }
        _instances.add(index, instance);
        _capacity += size;
        return true;
    }

    public void stop(int control) {
        for (Instance instance : _instances) {
            if (instance.control == control) {
                if (instance.streamId != 0) {
                    soundPool.stop(instance.streamId);
                    instance.streamId = 0;
                }
                instance.loop = false;
                instance.singleElapsed = 0;
            }
        }
    }

    public void pause(int control) {
        for (Instance instance : _instances) {
            if (instance.control == control && instance.streamId != 0) {
                soundPool.pause(instance.streamId);
            }
        }
    }

    public void resume(int control) {
        for (Instance instance : _instances) {
            if (instance.control == control && instance.streamId != 0) {
                soundPool.resume(instance.streamId);
            }
        }
    }

    public void setVolume(int category, float volume) {
        for (Instance instance : _instances) {
            if (instance.category == category && instance.streamId != 0) {
                float realVolume = instance.volume * volume;
                soundPool.setVolume(instance.streamId, realVolume, realVolume);
            }
        }
    }

    public void autoPause() {
        soundPool.autoPause();
    }

    public void autoResume() {
        soundPool.autoResume();
    }
}

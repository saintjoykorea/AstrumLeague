package kr.co.brgames.cdk;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;

import android.content.Context;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.os.Build;
import android.util.Log;

class CSAudioImpl implements Runnable {
    private final CSAudioSoundPool _soundPool;
    private final CSAudioMediaPlayer _mediaPlayer;
    private final Set<String> _soundPoolFailPaths;
    private final Map<Integer, Float> _volumes;
    private final ScheduledExecutorService _executor;
    private boolean _alive;

    public int singleCounter;

    public CSAudioImpl() {
        _soundPool = new CSAudioSoundPool();
        _mediaPlayer = new CSAudioMediaPlayer();
        _soundPoolFailPaths = new HashSet<>();
        _volumes = new HashMap<>();
        _alive = true;

        _executor = Executors.newSingleThreadScheduledExecutor();
        _executor.scheduleAtFixedRate(this, 0, 20, TimeUnit.MILLISECONDS);
    }

    public void dispose() {
        _alive = false;
    }

    private void play(int control, String path, float volume, boolean loop, int priority, int category, boolean fx, boolean single) {
        if (single) {
            if (_soundPool.isSinglePlaying() || _mediaPlayer.isSinglePlaying()) return;

            singleCounter++;
        }

        Float f = _volumes.get(category);

        float mainVolume = f != null ? f : 1.0f;

        loop:
        if (fx && !_soundPoolFailPaths.contains(path)) {
            int singleDuration = 0;
            if (single) {
                singleDuration = _mediaPlayer.getSingleDuration(path);
                if (singleDuration == 0) {
                    break loop;
                }
            }
            if (_soundPool.play(control, path, mainVolume, volume, loop, priority, category, singleDuration)) {
                return;
            }
            Log.w(CSAudio.TAG, String.format("sound pool fail:%s", path));
            _soundPoolFailPaths.add(path);
        }
        _mediaPlayer.play(control, path, mainVolume, volume, loop, priority, category, single);
    }

    private void stop(int control) {
        _soundPool.stop(control);
        _mediaPlayer.stop(control);
    }

    private void pause(int control) {
        _soundPool.pause(control);
        _mediaPlayer.pause(control);
    }

    private void resume(int control) {
        _soundPool.resume(control);
        _mediaPlayer.resume(control);
    }

    private void setVolume(int category, float volume) {
        if (volume == 1.0f) {
            _volumes.remove(category);
        }
        else {
            _volumes.put(category, volume);
        }
        _soundPool.setVolume(category, volume);
        _mediaPlayer.setVolume(category, volume);
    }

    private void autoPause() {
        _soundPool.autoPause();
        _mediaPlayer.autoPause();
    }

    private void autoResume() {
        _soundPool.autoResume();
        _mediaPlayer.autoResume();
    }

    @Override
    public void run() {
        while (_alive) {
            String command = CSAudio.nativeCommand();

            if (command == null) return;

            int s = 0;

            for (; ; ) {
                int e = command.indexOf(';', s);

                if (e < 0) break;

                StringTokenizer token = new StringTokenizer(command.substring(s, e), ",");
                String op = token.nextToken();

                switch (op) {
                    case "play":
                        {
                            int control = Integer.parseInt(token.nextToken());
                            String path = token.nextToken();
                            float volume = Float.parseFloat(token.nextToken());
                            boolean loop = Integer.parseInt(token.nextToken()) != 0;
                            int priority = Integer.parseInt(token.nextToken());
                            int category = Integer.parseInt(token.nextToken());
                            boolean fx = Integer.parseInt(token.nextToken()) != 0;
                            boolean single = Integer.parseInt(token.nextToken()) != 0;

                            play(control, path, volume, loop, priority, category, fx, single);
                        }
                        break;
                    case "stop":
                        {
                            int control = Integer.parseInt(token.nextToken());

                            stop(control);
                        }
                        break;
                    case "pause":
                        {
                            int control = Integer.parseInt(token.nextToken());

                            pause(control);
                        }
                        break;
                    case "resume":
                        {
                            int control = Integer.parseInt(token.nextToken());

                            resume(control);
                        }
                        break;
                    case "volume":
                        {
                            int category = Integer.parseInt(token.nextToken());
                            float volume = Float.parseFloat(token.nextToken());
                            setVolume(category, volume);
                        }
                        break;
                    case "autoPause":
                        autoPause();
                        break;
                    case "autoResume":
                        autoResume();
                        break;
                }
                s = e + 1;
            }
        }
        _soundPool.dispose();
        _mediaPlayer.dispose();
        _executor.shutdown();
    }
}

public class CSAudio {
    private static CSAudioImpl _impl;

    public static final String TAG = "CSAudio";

    public static void create() {
        _impl = new CSAudioImpl();
    }

    public static void dispose() {
        _impl.dispose();
        _impl = null;
    }

    public static int singleCounter() {
        return _impl.singleCounter;
    }

    public static void vibrate(final float time) {
        CSActivity.sharedActivity().queueEvent(()-> {
            Vibrator vibrator = (Vibrator) CSActivity.sharedActivity().getSystemService(Context.VIBRATOR_SERVICE);
            int millisecond = (int)(time * 1000);
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
                vibrator.vibrate(VibrationEffect.createOneShot(millisecond, VibrationEffect.DEFAULT_AMPLITUDE));
            } else {
                vibrator.vibrate(millisecond);
            }
        });
    }

    public static native String nativeCommand();
}


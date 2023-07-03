package kr.co.brgames.cdk;

import android.graphics.Bitmap;
import android.graphics.RectF;
import android.net.http.SslError;
import android.util.Base64;
import android.view.View;
import android.view.ViewGroup;
import android.view.ViewGroup.LayoutParams;
import android.webkit.SslErrorHandler;
import android.webkit.WebChromeClient;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.FrameLayout;
import android.widget.RelativeLayout;
import android.util.Log;

import java.util.HashMap;
import java.util.Map;

class CSWebView extends WebViewClient {
    private long _target;
    private WebView _webView;
    private RelativeLayout _layout;
    private RectF _frame;
    private boolean _loading;
    private boolean _scaleToFit;
    
    private static final String TAG = "CSWebView";

    public CSWebView(long target) {
        _target = target;

        CSActivity.sharedActivity().queueEvent(() -> {
            _webView = new WebView(CSActivity.sharedActivity());
            _webView.setWebViewClient(CSWebView.this);
            _webView.setWebChromeClient(new WebChromeClient());
            _webView.getSettings().setLoadsImagesAutomatically(true);
            _webView.getSettings().setJavaScriptEnabled(true);
            _webView.getSettings().setAllowFileAccess(true);
            _webView.setScrollBarStyle(View.SCROLLBARS_INSIDE_OVERLAY);
            _webView.getSettings().setJavaScriptCanOpenWindowsAutomatically(true);
            _webView.getSettings().setMediaPlaybackRequiresUserGesture(false);
            _webView.getSettings().setAppCacheEnabled(true);
            _webView.getSettings().setDatabaseEnabled(true);
            _webView.getSettings().setDomStorageEnabled(true);
            _webView.getSettings().setCacheMode(WebSettings.LOAD_NO_CACHE);

            _webView.setBackgroundColor(android.graphics.Color.TRANSPARENT);
            _webView.getSettings().setMixedContentMode(WebSettings.MIXED_CONTENT_ALWAYS_ALLOW);

            _layout = new RelativeLayout(CSActivity.sharedActivity());
            _layout.setBackgroundColor(android.graphics.Color.TRANSPARENT);
            _layout.setLayoutParams(new FrameLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT));
            _layout.addView(_webView);
        });
    }

    public void addToView() {
        CSActivity.sharedActivity().queueEvent(() -> {
            ViewGroup parent = CSActivity.sharedActivity().viewGroup();

            parent.addView(_layout);
        });
    }

    private void removeFromView(final boolean released) {
        CSActivity.sharedActivity().queueEvent(() -> {
            _webView.clearFocus();
            _webView.stopLoading();

            CSActivity.sharedActivity().viewGroup().removeView(_layout);

            if (released) {
                _webView.setWebViewClient(null);
                _webView.destroy();
            }
        });
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
            _webView.setLayoutParams(params);
        });
    }
    
    public void setFrame(float x, float y, float width, float height) {
        setFrame(new RectF(x, y, x + width, y + height));
    }

    public void loadData(byte[] data, final String mimeType, final String textEncodingName, final String baseUrl) {
        final String strdata = Base64.encodeToString(data, Base64.DEFAULT);

        CSActivity.sharedActivity().queueEvent(() -> _webView.loadDataWithBaseURL(baseUrl, strdata, mimeType, textEncodingName, null));
    }

    public void loadHTML(final String html, final String baseUrl) {
        CSActivity.sharedActivity().queueEvent(() -> _webView.loadDataWithBaseURL(baseUrl, html, "text/html", "UTF-8", null));
    }

    public void loadRequest(final CSURLRequest request) {
        _loading = true;

        CSActivity.sharedActivity().queueEvent(() -> _webView.loadUrl(request.URL(), request.headers()));
    }

    @Override
    public void onPageFinished(WebView view, String url) {
        _loading = false;

        if (_target != 0) {
            CSActivity.sharedActivity().view().queueEvent(() -> {
                if (_target != 0) {
                    nativeFinishLoad(_target);
                }
            });
        }
    }

    @Override
    public void onPageStarted(WebView view, String url, Bitmap favicon) {
        _loading = true;

        if (_target != 0) {
            CSActivity.sharedActivity().view().queueEvent(() -> {
                if (_target != 0) {
                    nativeStartLoad(_target);
                }
            });
        }
    }

    @Override
    public void onReceivedError(WebView view, int errorCode, String description, String failingUrl) {
        onError();
    }

    @Override
    public void onReceivedSslError(WebView view, SslErrorHandler handler, SslError error) {
        onError();
    }
    
    private void onError() {
        CSActivity.sharedActivity().view().queueEvent(() -> {
            _loading = false;

            if (_target != 0) {
                nativeLoadError(_target);
            }
        });
    }

    private boolean nativeLinkReturn;

    @Override
    public boolean shouldOverrideUrlLoading(WebView view, final String url) {
        Runnable r = () -> {
            nativeLinkReturn = _target != 0 && nativeLink(_target, url);
            synchronized (CSWebView.this) {
                CSWebView.this.notify();
            }
        };

        synchronized (this) {
            CSActivity.sharedActivity().view().queueEvent(r);
            try {
                wait();
            }
            catch (InterruptedException e) {
                Log.e(TAG, e.getMessage(), e);
            }
        }
        return !nativeLinkReturn;
    }

    public boolean isLoading() {
        return _loading;
    }

    public void stopLoading() {
        _loading = false;

        CSActivity.sharedActivity().queueEvent(() -> _webView.stopLoading());
    }

    public void reload() {
        CSActivity.sharedActivity().queueEvent(() -> _webView.reload());
    }

    public boolean canGoBack() {
        return _webView.canGoBack();
    }

    public boolean canGoForward() {
        return _webView.canGoForward();
    }

    public void goBack() {
        CSActivity.sharedActivity().queueEvent(() -> _webView.goBack());
    }

    public void goForward() {
        CSActivity.sharedActivity().queueEvent(() -> _webView.goForward());
    }

    public boolean scaleToFit() {
        return _scaleToFit;
    }

    public void setScaleToFit(boolean scaleToFit) {
        _scaleToFit = scaleToFit;

        CSActivity.sharedActivity().queueEvent(() -> {
            _webView.getSettings().setLoadWithOverviewMode(_scaleToFit);
            _webView.getSettings().setUseWideViewPort(_scaleToFit);
        });
    }
    
    public static native void nativeLoadError(long target);
    public static native boolean nativeLink(long target, String url);
    public static native void nativeStartLoad(long target);
    public static native void nativeFinishLoad(long target);
}

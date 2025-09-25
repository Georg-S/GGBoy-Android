package com.example.ggboy;

import android.graphics.Bitmap;
import android.view.SurfaceHolder;
import android.graphics.Canvas;
import android.graphics.Rect;
import android.graphics.Color;


import androidx.annotation.NonNull;

import java.util.concurrent.atomic.AtomicBoolean;

public class Renderer implements SurfaceHolder.Callback
{
    private static final int GAMEBOY_IMAGE_WIDTH = 160;
    private static final int GAMEBOY_IMAGE_HEIGHT = 144;
    private static final int UPDATE_MESSAGES_AFTER_STEPS = 30;
    private AtomicBoolean landscape = new AtomicBoolean(false);
    private SurfaceHolder holder;
    private Thread thread;
    private volatile boolean isRunning = false;
    private android.os.Handler mainThread = null;
    // Not sure how the access of this works with threading, therefore this should only be used when running some code in the main thread
    private MainActivity mainActivity = null;
    // We don't want to update the messages in every tick (performance overhead)
    private int updateMessagesCounter = 0;

    public Renderer(MainActivity activity)
    {
        this.mainActivity = activity;
    }

    public void setMainThread(android.os.Handler handler)
    {
        this.mainThread = handler;
    }

    public void setLandscape(boolean value)
    {
        landscape.set(value);
    }

    public void run()
    {
        thread = new Thread(() ->
        {
            while (isRunning)
            {
                runRenderer();
                handleEmulatorMessages();

                try {
                    Thread.sleep(1);
                } catch (InterruptedException ignored) {
                    isRunning = false;
                }
            }
        });
        thread.start();
    }

    // Checks for and displays errors / warnings / infos coming from the emulator thread
    private void handleEmulatorMessages()
    {
        updateMessagesCounter++;
        if (updateMessagesCounter >= UPDATE_MESSAGES_AFTER_STEPS)
        {
            updateMessagesCounter = 0;
            updateMessages();
            var infos = getInfos();
            var warnings = getWarnings();
            var errors = getErrors();
            for (var info : infos)
            {
                mainThread.post(()->
                {
                    mainActivity.displayInfo(info);
                });
            }
            for (var warning : warnings)
            {
                mainThread.post(()->
                {
                    mainActivity.displayWarning(warning);
                });
            }
            for (var error : errors)
            {
                mainThread.post(()->
                {
                    mainActivity.displayError(error);
                });
            }
        }
    }

    private float calculateMaxUniformScale(float screenWidth, float screenHeight)
    {
        float fac1 = screenWidth / GAMEBOY_IMAGE_WIDTH;
        float fac2 = screenHeight / GAMEBOY_IMAGE_HEIGHT;
        return Math.min(fac1, fac2);
    }

    public void renderEmulatorImage(byte[] rgbBytes)
    {
        if (rgbBytes.length == 0)
            return;

        final boolean isInLandscape = landscape.get();

        Canvas canvas = holder.lockCanvas();
        int screenWidth = canvas.getWidth();
        int screenHeight = canvas.getHeight();
        final float scaling = calculateMaxUniformScale(screenWidth, screenHeight);

        Bitmap bitmap = Bitmap.createBitmap(GAMEBOY_IMAGE_WIDTH, GAMEBOY_IMAGE_HEIGHT, Bitmap.Config.ARGB_8888);

        for (int y = 0; y < GAMEBOY_IMAGE_HEIGHT; y++)
        {
            for (int x = 0; x < GAMEBOY_IMAGE_WIDTH; x++)
            {
                int offset = (y * GAMEBOY_IMAGE_WIDTH + x) * 4;
                int red = rgbBytes[offset] & 0xFF;
                int green = rgbBytes[offset + 1] & 0xFF;
                int blue = rgbBytes[offset + 2] & 0xFF;
                int color = Color.rgb(red, green, blue);
                bitmap.setPixel(x, y, color);
            }
        }

        int imageWidth = (int) (GAMEBOY_IMAGE_WIDTH * scaling);
        int imageHeight = (int) (GAMEBOY_IMAGE_HEIGHT * scaling);
        int startX = 0;
        if (isInLandscape)
            startX = (screenWidth - imageWidth) / 2;

        Rect destRect = new Rect(startX, 0, startX + imageWidth, imageHeight);
        canvas.drawBitmap(bitmap, null, destRect, null);

        holder.unlockCanvasAndPost(canvas);
    }

    public void stop()
    {
        isRunning = false;
        if (thread.isAlive())
        {
            try
            {
                thread.join();
            } catch (InterruptedException e)
            {
                e.printStackTrace();
            }
        }
    }

    @Override
    public void surfaceCreated(@NonNull SurfaceHolder holder)
    {
        this.holder = holder;
        if (!isRunning)
        {
            isRunning = true;
            run();
        }
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height)
    {
        this.holder = holder;
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder)
    {
        stop();
    }

    public native void runRenderer();
    private native void updateMessages();
    public native String[] getInfos();
    public native String[] getWarnings();
    public native String[] getErrors();
}

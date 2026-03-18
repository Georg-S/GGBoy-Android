package com.example.ggboy;

import android.graphics.Canvas;
import android.graphics.Rect;
import android.util.DisplayMetrics;
import android.view.SurfaceView;
import android.view.View;

import androidx.constraintlayout.widget.ConstraintLayout;

import java.util.concurrent.atomic.AtomicBoolean;

public class MainActivityLayouting
{
    private static final int GAMEBOY_IMAGE_WIDTH = 160;
    private static final int GAMEBOY_IMAGE_HEIGHT = 144;
    private final MainActivity mainActivity;
    private int emulatorImageStartX = 0;
    private int emulatorImageWidth = 0;
    private int emulatorImageHeight = 0;
    private final AtomicBoolean landscape = new AtomicBoolean(false);

    MainActivityLayouting(MainActivity activity)
    {
        mainActivity = activity;
    }

    public void setLandscape(boolean value)
    {
        landscape.set(value);
    }

    public boolean isLandscape()
    {
        return landscape.get();
    }

    public int getEmulatorImageStartX()
    {
        return emulatorImageStartX;
    }

    public int getEmulatorImageWidth()
    {
        return emulatorImageWidth;
    }

    public int getEmulatorImageHeight()
    {
        return emulatorImageHeight;
    }

    private void applyPortraitLayout()
    {
        View dpad = mainActivity.findViewById(R.id.dpad);
        View aButton = mainActivity.findViewById(R.id.a_button);
        View bButton = mainActivity.findViewById(R.id.b_button);
        View startButton = mainActivity.findViewById(R.id.start_button);
        View selectButton = mainActivity.findViewById(R.id.select_button);

        ConstraintLayout constraintLayout = mainActivity.findViewById(R.id.constraint);

        DisplayMetrics metrics = new DisplayMetrics();
        mainActivity.getWindowManager().getDefaultDisplay().getRealMetrics(metrics);
        View decorView = mainActivity.getWindow().getDecorView();
        Rect rect = new Rect();
        decorView.getWindowVisibleDisplayFrame(rect);
        final int screenWidth = constraintLayout.getWidth();
        final int screenHeight = constraintLayout.getHeight();
        final int remainingHeight = screenHeight - emulatorImageHeight;

        // DPAD
        ConstraintLayout.LayoutParams params = (ConstraintLayout.LayoutParams) dpad.getLayoutParams();
        final int dpadSize = (int) (0.4f * screenWidth);
        final int dpadLeftMargin = (int) (0.05f * screenWidth);
        final int dpadTopMargin = emulatorImageHeight + Math.max(20, (remainingHeight - dpadSize) / 3); // Divided by 3 is a somewhat arbitrary value

        params.width = dpadSize;
        params.height = dpadSize;
        params.leftMargin = dpadLeftMargin;
        params.topMargin = dpadTopMargin;
        params.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
        params.leftToLeft = ConstraintLayout.LayoutParams.PARENT_ID;
        dpad.setLayoutParams(params);

        // Shared: START & SELECT
        final int startSelectButtonWidth = (int) (0.25f * screenWidth);
        final int startSelectButtonHeight = (int)(startSelectButtonWidth / 2.666);

        // Position them below the emulator image, centered horizontally
        final int dpadEnd = dpadTopMargin + dpadSize;
        final int remainingTopSpace = screenHeight - dpadEnd;
        int startTopMargin = dpadEnd + (remainingTopSpace / 2);
        startTopMargin = Math.min(startTopMargin, screenHeight - startSelectButtonHeight);

        // START
        ConstraintLayout.LayoutParams startParams = (ConstraintLayout.LayoutParams) startButton.getLayoutParams();
        startParams.width = startSelectButtonWidth;
        startParams.height = startSelectButtonHeight;
        startParams.topMargin = startTopMargin;
        startParams.leftMargin = (screenWidth / 2)  + 20;
        startParams.leftToLeft = ConstraintLayout.LayoutParams.PARENT_ID;
        startParams.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
        startButton.setLayoutParams(startParams);

        // SELECT
        ConstraintLayout.LayoutParams selectParams = (ConstraintLayout.LayoutParams) selectButton.getLayoutParams();
        selectParams.width = startSelectButtonWidth;
        selectParams.height = startSelectButtonHeight;
        selectParams.topMargin = startTopMargin;
        selectParams.leftMargin = (screenWidth / 2) - startSelectButtonWidth - 20;
        selectParams.leftToLeft = ConstraintLayout.LayoutParams.PARENT_ID;
        selectParams.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
        selectButton.setLayoutParams(selectParams);

        // --- A & B BUTTONS ---
        final int abSize = (int) (0.175 * screenWidth);
        final int abTopMargin = dpadTopMargin;
        final int abRightMargin = (int) (0.05f * screenWidth);

        // A button
        ConstraintLayout.LayoutParams aParams = (ConstraintLayout.LayoutParams) aButton.getLayoutParams();
        aParams.width = abSize;
        aParams.height = abSize;
        aParams.topMargin = abTopMargin;
        aParams.rightMargin = abRightMargin;
        aParams.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
        aParams.rightToRight = ConstraintLayout.LayoutParams.PARENT_ID;
        aButton.setLayoutParams(aParams);

        // B button
        ConstraintLayout.LayoutParams bParams = (ConstraintLayout.LayoutParams) bButton.getLayoutParams();
        bParams.width = abSize;
        bParams.height = abSize;
        bParams.topMargin = abTopMargin + (int)(0.04f * screenHeight);
        bParams.rightMargin = abRightMargin + (int)(abSize * 1.2); // offset left from A
        bParams.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
        bParams.rightToRight = ConstraintLayout.LayoutParams.PARENT_ID;
        bButton.setLayoutParams(bParams);
    }

    private void applyLandscapeLayout()
    {
        View dpad = mainActivity.findViewById(R.id.dpad);
        View aButton = mainActivity.findViewById(R.id.a_button);
        View bButton = mainActivity.findViewById(R.id.b_button);
        View startButton = mainActivity.findViewById(R.id.start_button);
        View selectButton = mainActivity.findViewById(R.id.select_button);
        ConstraintLayout constraintLayout = mainActivity.findViewById(R.id.constraint);

        final int screenWidth = constraintLayout.getWidth();
        final int screenHeight = constraintLayout.getHeight();
        // compute left/right emulator image is centered
        final int leftStrip = Math.max(0, emulatorImageStartX);
        final int rightStrip = Math.max(0, screenWidth - (emulatorImageStartX + emulatorImageWidth));
        final int minTouch = dpToPx(48);            // arbitrary minimum touch target
        final int centerY = screenHeight / 2;

        // DPAD on left strip
        final int dpadSize = (int) (0.80f * leftStrip);
        final int leftPadding = Math.max(dpToPx(8), (int)(0.05f * leftStrip));

        ConstraintLayout.LayoutParams dpadParams = (ConstraintLayout.LayoutParams) dpad.getLayoutParams();
        dpadParams.width = dpadSize;
        dpadParams.height = dpadSize;
        dpadParams.leftMargin = Math.max(4, leftPadding);
        dpadParams.topMargin = Math.max(0, centerY - (dpadSize / 2));
        dpadParams.leftToLeft = ConstraintLayout.LayoutParams.PARENT_ID;
        dpadParams.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
        dpad.setLayoutParams(dpadParams);

        // A & B
        int abSize = (int) (0.45f * rightStrip);
        abSize = Math.max(abSize, minTouch);
        // ensure AB doesn't exceed strip
        abSize = Math.min(abSize, (int)(0.9f * rightStrip));

        final int abRightPadding = Math.max(dpToPx(8), (int)(0.06f * rightStrip));
        final int abDistance = (int) (0.8 * abSize);
        final int aTop = Math.max(0, centerY - (abSize + dpToPx(6)));
        final int bTop = Math.min(screenHeight, aTop + abDistance);

        ConstraintLayout.LayoutParams aParams = (ConstraintLayout.LayoutParams) aButton.getLayoutParams();
        aParams.width = abSize;
        aParams.height = abSize;
        aParams.topMargin = aTop;
        aParams.rightMargin = Math.max(4, abRightPadding);
        aParams.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
        aParams.rightToRight = ConstraintLayout.LayoutParams.PARENT_ID;
        aButton.setLayoutParams(aParams);

        ConstraintLayout.LayoutParams bParams = (ConstraintLayout.LayoutParams) bButton.getLayoutParams();
        bParams.width = abSize;
        bParams.height = abSize;
        bParams.topMargin = bTop;
        bParams.rightMargin = Math.max(4, abRightPadding + abDistance);
        bParams.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
        bParams.rightToRight = ConstraintLayout.LayoutParams.PARENT_ID;
        bButton.setLayoutParams(bParams);

        // Start & Select
        int startSelectWidth = Math.max(minTouch, (int)(0.45f * abSize));
        int startSelectHeight = Math.max(minTouch / 2, (int)(startSelectWidth / 1.5f));
        startSelectWidth = (int)(0.8f * rightStrip);

        final int startSelectTop = screenHeight - dpToPx(8) - startSelectHeight;
        final int startSelectLeftRightMargin = Math.max((leftStrip / 2) - (startSelectWidth / 2), dpToPx(8));

        ConstraintLayout.LayoutParams startParams = (ConstraintLayout.LayoutParams) startButton.getLayoutParams();
        startParams.width = startSelectWidth;
        startParams.height = startSelectHeight;
        startParams.topMargin = startSelectTop;
        startParams.rightMargin = startSelectLeftRightMargin;
        startParams.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
        startParams.rightToRight = ConstraintLayout.LayoutParams.PARENT_ID;
        startButton.setLayoutParams(startParams);

        ConstraintLayout.LayoutParams selectParams = (ConstraintLayout.LayoutParams) selectButton.getLayoutParams();
        selectParams.width = startSelectWidth;
        selectParams.height = startSelectHeight;
        selectParams.topMargin = startSelectTop;
        selectParams.leftMargin = startSelectLeftRightMargin;
        selectParams.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
        selectParams.leftToLeft = ConstraintLayout.LayoutParams.PARENT_ID;
        selectButton.setLayoutParams(selectParams);
    }

    private int dpToPx(int dp)
    {
        float density = mainActivity.getResources().getDisplayMetrics().density;
        return Math.round(dp * density);
    }

    private float calculateMaxUniformScale(float screenWidth, float screenHeight)
    {
        float fac1 = screenWidth / GAMEBOY_IMAGE_WIDTH;
        float fac2 = screenHeight / GAMEBOY_IMAGE_HEIGHT;
        return Math.min(fac1, fac2);
    }

    public void updateEmulatorScreenSize(boolean isInLandscape)
    {
        SurfaceView surfaceView = mainActivity.findViewById(R.id.emulator_surface);
        var holder = surfaceView.getHolder();
        Canvas canvas = holder.lockCanvas();
        if (canvas == null)
            return;

        final int screenWidth = canvas.getWidth();
        final int screenHeight = canvas.getHeight();
        final float scaling = calculateMaxUniformScale(screenWidth, screenHeight);

        emulatorImageWidth = (int) (GAMEBOY_IMAGE_WIDTH * scaling);
        emulatorImageHeight = (int) (GAMEBOY_IMAGE_HEIGHT * scaling);
        emulatorImageStartX = 0;
        if (isInLandscape)
            emulatorImageStartX = (screenWidth - emulatorImageWidth) / 2;

        holder.unlockCanvasAndPost(canvas);
    }

    public void applyLayout()
    {
        updateEmulatorScreenSize(landscape.get());
        if (landscape.get())
            applyLandscapeLayout();
        else
            applyPortraitLayout();
    }
}

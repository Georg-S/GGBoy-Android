package com.example.ggboy;

import android.graphics.Canvas;
import android.graphics.Rect;
import android.util.DisplayMetrics;
import android.view.SurfaceView;
import android.view.View;

import androidx.constraintlayout.widget.ConstraintLayout;

public class MainActivityLayouting
{
    private static final int GAMEBOY_IMAGE_WIDTH = 160;
    private static final int GAMEBOY_IMAGE_HEIGHT = 144;
    private MainActivity mainActivity = null;
    private int emulatorImageStartX = 0;
    private int emulatorImageWidth = 0;
    private int emulatorImageHeight = 0;

    MainActivityLayouting(MainActivity activity)
    {
        mainActivity = activity;
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

        ConstraintLayout constraintLayout = mainActivity.findViewById(androidx.constraintlayout.widget.R.id.constraint);

        DisplayMetrics metrics = new DisplayMetrics();
        mainActivity.getWindowManager().getDefaultDisplay().getRealMetrics(metrics);
        View decorView = mainActivity.getWindow().getDecorView();
        Rect rect = new Rect();
        decorView.getWindowVisibleDisplayFrame(rect);
        int screenWidth = constraintLayout.getWidth();
        int screenHeight = constraintLayout.getHeight();
        int remainingHeight = screenHeight - emulatorImageHeight;

        // DPAD
        ConstraintLayout.LayoutParams params = (ConstraintLayout.LayoutParams) dpad.getLayoutParams();

        // Calculate desired sizes and margins
        int dpadSize = (int) (0.4f * screenWidth);
        int leftMargin = (int) (0.05f * screenWidth);
//        int topMargin = emulatorImageHeight + (int)(0.04f * screenHeight);
        int dpadTopMargin = emulatorImageHeight + Math.max(20, (remainingHeight - dpadSize) / 3); // Divided by 3 is a somewhat arbitrary value

        params.width = dpadSize;
        params.height = dpadSize;
        params.leftMargin = leftMargin;
        params.topMargin = dpadTopMargin;
        params.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
        params.leftToLeft = ConstraintLayout.LayoutParams.PARENT_ID;
        dpad.setLayoutParams(params);

        // --- START & SELECT ---
        // Common size for start/select
        int buttonWidth = (int) (0.25f * screenWidth);
        int buttonHeight = (int)(buttonWidth / 2.666);

        // Position them below the emulator image, centered horizontally
        int startBottomMargin = (int) (0.01f * screenHeight);
        int dpadEnd = dpadTopMargin + dpadSize;
        int remainingTopSpace = screenHeight - dpadEnd;
        int startTopMargin = dpadEnd + (remainingTopSpace / 2);
        startTopMargin = Math.min(startTopMargin, screenHeight - buttonHeight);

        // START
        ConstraintLayout.LayoutParams startParams = (ConstraintLayout.LayoutParams) startButton.getLayoutParams();
        startParams.width = buttonWidth;
        startParams.height = buttonHeight;
//        startParams.bottomMargin = startBottomMargin;
        startParams.topMargin = startTopMargin;
        startParams.leftMargin = (screenWidth / 2)  + 20;
        startParams.leftToLeft = ConstraintLayout.LayoutParams.PARENT_ID;
        startParams.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
        startButton.setLayoutParams(startParams);

        // SELECT
        ConstraintLayout.LayoutParams selectParams = (ConstraintLayout.LayoutParams) selectButton.getLayoutParams();
        selectParams.width = buttonWidth;
        selectParams.height = buttonHeight;
        selectParams.topMargin = startTopMargin;
//        selectParams.bottomMargin = startBottomMargin;
        selectParams.leftMargin = (screenWidth / 2) - buttonWidth - 20;
        selectParams.leftToLeft = ConstraintLayout.LayoutParams.PARENT_ID;
        selectParams.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
        selectButton.setLayoutParams(selectParams);

        // --- A & B BUTTONS ---
        int abSize = (int) (0.175 * screenWidth);
        int abTopMargin = dpadTopMargin;
        int abRightMargin = (int) (0.05f * screenWidth);

        // A button
        ConstraintLayout.LayoutParams aParams =
                (ConstraintLayout.LayoutParams) aButton.getLayoutParams();
        aParams.width = abSize;
        aParams.height = abSize;
        aParams.topMargin = abTopMargin;
        aParams.rightMargin = abRightMargin;
        aParams.topToTop = ConstraintLayout.LayoutParams.PARENT_ID;
        aParams.rightToRight = ConstraintLayout.LayoutParams.PARENT_ID;
        aButton.setLayoutParams(aParams);

        // B button
        ConstraintLayout.LayoutParams bParams =
                (ConstraintLayout.LayoutParams) bButton.getLayoutParams();
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
        // For now do nothing just use the layout from the XML
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

        int screenWidth = canvas.getWidth();
        int screenHeight = canvas.getHeight();
        final float scaling = calculateMaxUniformScale(screenWidth, screenHeight);

        emulatorImageWidth = (int) (GAMEBOY_IMAGE_WIDTH * scaling);
        emulatorImageHeight = (int) (GAMEBOY_IMAGE_HEIGHT * scaling);
        emulatorImageStartX = 0;
        if (isInLandscape)
            emulatorImageStartX = (screenWidth - emulatorImageWidth) / 2;

        holder.unlockCanvasAndPost(canvas);
    }

    public void applyLayout(boolean landscape)
    {
        updateEmulatorScreenSize(landscape);
        if (landscape)
            applyLandscapeLayout();
        else
            applyPortraitLayout();
    }
}

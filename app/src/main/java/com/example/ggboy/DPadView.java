package com.example.ggboy;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.RectF;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;

public class DPadView extends View
{
    static private final int pressedColor = Color.argb(255, 150, 150, 150);
    static private final int notPressedColor = Color.argb(255, 100, 100, 100);
    static private final int circleColor = Color.argb(255, 70, 70, 70);
    private Paint paint;
    private RectF upRect, downRect, leftRect, rightRect, centerRect;
    private boolean upPressed, downPressed, leftPressed, rightPressed;
    private float centerX, centerY, buttonSize;
    // Needed for accessing JNI methods
    private MainActivity activity;

    public DPadView(Context context, AttributeSet attrs)
    {
        super(context, attrs);
        init();
    }

    public void setActivity(MainActivity activity)
    {
        this.activity = activity;
    }

    private void init()
    {
        paint = new Paint();
        paint.setAntiAlias(true);
        paint.setStyle(Paint.Style.FILL);
        paint.setColor(notPressedColor);
    }

    @Override
    protected void onSizeChanged(int w, int h, int oldw, int oldh)
    {
        super.onSizeChanged(w, h, oldw, oldh);
        centerX = w / 2f;
        centerY = h / 2f;
        buttonSize = Math.min(w, h) / 3f;

        upRect = new RectF(centerX - buttonSize / 2, 0, centerX + buttonSize / 2, buttonSize);
        downRect = new RectF(centerX - buttonSize / 2, h - buttonSize, centerX + buttonSize / 2, h);
        leftRect = new RectF(0, centerY - buttonSize / 2, buttonSize, centerY + buttonSize / 2);
        rightRect = new RectF(w - buttonSize, centerY - buttonSize / 2, w, centerY + buttonSize / 2);
        centerRect = new RectF(centerX - buttonSize / 2, centerY - buttonSize / 2, centerX + buttonSize / 2, centerY + buttonSize / 2);
    }

    @Override
    protected void onDraw(Canvas canvas)
    {
        super.onDraw(canvas);

        drawDirection(canvas, upRect, upPressed);
        drawDirection(canvas, downRect, downPressed);
        drawDirection(canvas, leftRect, leftPressed);
        drawDirection(canvas, rightRect, rightPressed);
        paint.setColor(notPressedColor);
        canvas.drawRect(centerRect, paint);
        paint.setColor(circleColor);
        canvas.drawCircle(centerX, centerY, buttonSize * 0.4f, paint);
    }

    private void drawDirection(Canvas canvas, RectF rect, boolean pressed)
    {
        paint.setColor(pressed ? pressedColor : notPressedColor);
        canvas.drawRoundRect(rect, 10, 10, paint);
    }

    private void updateDDPadStates()
    {
        activity.setButtonState(GGBoyButton.UP.getValue(), upPressed);
        activity.setButtonState(GGBoyButton.DOWN.getValue(), downPressed);
        activity.setButtonState(GGBoyButton.RIGHT.getValue(), rightPressed);
        activity.setButtonState(GGBoyButton.LEFT.getValue(), leftPressed);
    }

    @Override
    public boolean performClick()
    {
        super.performClick();
        updateDDPadStates();
        return true;
    }

    private boolean handleInputChange(float x, float y)
    {
        boolean previousUp = upPressed;
        boolean previousDown = downPressed;
        boolean previousLeft = leftPressed;
        boolean previousRight = rightPressed;

        upPressed = upRect.contains(x, y);
        downPressed = downRect.contains(x, y);
        leftPressed = leftRect.contains(x, y);
        rightPressed = rightRect.contains(x, y);

        return previousUp != upPressed || previousDown != downPressed
                || previousLeft != leftPressed || previousRight != rightPressed;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        int action = event.getActionMasked();
        float x = event.getX();
        float y = event.getY();

        switch (action)
        {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_MOVE:
                if (handleInputChange(x, y))
                    performClick();
                invalidate();
                return true;

            case MotionEvent.ACTION_UP:
                upPressed = downPressed = leftPressed = rightPressed = false;
                updateDDPadStates();
                invalidate();
                return true;
        }
        return super.onTouchEvent(event);
    }
}
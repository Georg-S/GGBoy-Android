package com.example.ggboy;

import android.content.Context;
import android.content.res.TypedArray;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.GradientDrawable;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.MotionEvent;

import androidx.appcompat.widget.AppCompatButton;

public class GameButton extends AppCompatButton
{
    static private final int pressedColor = Color.argb(255, 150, 150, 150);
    static private final int notPressedColor = Color.argb(255, 100, 100, 100);
    private GGBoyButton button;
    MainActivity activity;
    private int shape;
    // Shape constants
    public static final int SHAPE_OVAL = 0;
    public static final int SHAPE_RECTANGLE = 1;

    public GameButton(Context context, AttributeSet attrs)
    {
        super(context, attrs);

        try (TypedArray a = context.obtainStyledAttributes(attrs, R.styleable.GameButton))
        {
            shape = a.getInt(R.styleable.GameButton_buttonShape, SHAPE_OVAL);
            a.recycle();
        }

        setBackground(createButtonBackground());
        setTextColor(Color.WHITE);
        setTransformationMethod(null); // Disable all caps
    }

    public void init(MainActivity activity, GGBoyButton button)
    {
        this.activity = activity;
        this.button = button;
    }

    private Drawable createButtonBackground()
    {
        GradientDrawable drawable = new GradientDrawable();
        drawable.setShape(shape == SHAPE_OVAL ?
                GradientDrawable.OVAL : GradientDrawable.RECTANGLE);

        drawable.setColor(notPressedColor);
        drawable.setStroke(2, Color.WHITE);

        if (shape == SHAPE_RECTANGLE)
        {
            drawable.setCornerRadius(8f);
        }

        return drawable;
    }

    @Override
    public boolean onTouchEvent(MotionEvent event)
    {
        int action = event.getActionMasked();
        switch (action)
        {
            case MotionEvent.ACTION_DOWN:
            case MotionEvent.ACTION_MOVE:
                performClick();
                setPressedState(true);
                return true;

            case MotionEvent.ACTION_UP:
                setPressedState(false);
                return true;
        }
        return super.onTouchEvent(event);
    }

    public void setPressedState(boolean pressed)
    {
        activity.setButtonState(button.getValue(), pressed);
        ((GradientDrawable) getBackground()).setColor(pressed ? pressedColor : notPressedColor);
        invalidate();
    }
}
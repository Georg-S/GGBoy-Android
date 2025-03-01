package com.example.ggboy;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.TextView;

import com.example.ggboy.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity
{
    // Used to load the 'androidtest' library on application startup.
    static
    {
        System.loadLibrary("GGBoy-Android");
    }

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        
        // Example of a call to a native method
        TextView tv = binding.sampleText;
        tv.setText(stringFromJNI());
    }

    /**
     * A native method that is implemented by the 'androidtest' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
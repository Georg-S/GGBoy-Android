package com.example.ggboy;

import androidx.activity.result.ActivityResultLauncher;
import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBar;
import androidx.appcompat.app.AppCompatActivity;

import android.content.res.Configuration;
import android.net.Uri;
import com.example.ggboy.GGBoyButton;

import androidx.activity.result.contract.ActivityResultContracts;

import android.view.Menu;
import android.view.MenuItem;
import android.os.Bundle;
import android.content.Intent;
import android.view.MotionEvent;
import android.view.SurfaceView;
import android.view.Window;
import android.widget.Button;
import android.widget.Toast;




public class MainActivity extends AppCompatActivity
{
    private DPadView dpad;
    //    private GameButton a_button;
    private static String ROM_PATH = "ROMs";
    private Renderer renderer = null;
    private ActivityResultLauncher<Intent> romPickerLauncher;
    private ActivityResultLauncher<Intent> addRomLauncher;

    // Used to load the 'GGBoy-Android' library on application startup.
    static
    {
        System.loadLibrary("GGBoy-Android");
    }

    private void registerAddRomLauncher()
    {
        addRomLauncher = registerForActivityResult(
                new ActivityResultContracts.StartActivityForResult(),
                result ->
                {
                    if (result.getResultCode() != RESULT_OK || result.getData() == null)
                        return;
                    Uri uri = result.getData().getData();
                    if (uri == null)
                        return;

                    Utility.copyFileToInternalFolder(this, ROM_PATH, uri);
                }
        );
    }

    private void registerFilePickerLauncher()
    {
        romPickerLauncher = registerForActivityResult(
                new ActivityResultContracts.StartActivityForResult(),
                result ->
                {
                    if (result.getResultCode() != RESULT_OK || result.getData() == null)
                    {
                        Toast.makeText(this, "No ROMs found", Toast.LENGTH_SHORT).show();
                        return;
                    }
                    String selectedFile = result.getData().getStringExtra("selectedFile");
                    loadROM(selectedFile);
                }
        );
    }

    private void openRom()
    {
        Intent intent = new Intent(this, FilePickerActivity.class);
        intent.putExtra(FilePickerActivity.EXTRA_DIRECTORY, ROM_PATH);
        romPickerLauncher.launch(intent);
    }

    /// Adds the ROM to the internal storage, the C++ side can then access the internal storage without permission hassle
    private void addRom()
    {
        Intent intent = new Intent(Intent.ACTION_OPEN_DOCUMENT);
        intent.addCategory(Intent.CATEGORY_OPENABLE);
        intent.setType("*/*");
        addRomLauncher.launch(intent);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item)
    {
        int id = item.getItemId();
        if (id == R.id.menu_open_rom)
            openRom();
        if (id == R.id.menu_add_rom)
            addRom();

        return true;
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        getMenuInflater().inflate(R.menu.emulator_menu, menu);
        return true;
    }

    private void initUI()
    {
        SurfaceView surfaceView = findViewById(R.id.emulator_surface);
        surfaceView.getHolder().addCallback(renderer);

        dpad = findViewById(R.id.dpad);
        dpad.setActivity(this);

        GameButton a_button = findViewById(R.id.a_button);
        GameButton b_button = findViewById(R.id.b_button);
        GameButton start_button = findViewById(R.id.start_button);
        GameButton select_button = findViewById(R.id.select_button);
        a_button.init(this, GGBoyButton.A);
        b_button.init(this, GGBoyButton.B);
        start_button.init(this, GGBoyButton.START);
        select_button.init(this, GGBoyButton.SELECT);
    }

    @Override
    public void onConfigurationChanged(@NonNull Configuration newConfig)
    {
            super.onConfigurationChanged(newConfig);

            if (renderer != null)
                renderer.stop();

            ActionBar bar = getSupportActionBar();
            final boolean isLandscape = newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE;
            setContentView(R.layout.activity_main);

            if (renderer != null)
                renderer.setLandscape(isLandscape);

            if (bar != null)
            {
                if (isLandscape)
                    bar.hide();
                else
                    bar.show();
            }

            initUI();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main); // Set initial layout

        renderer = new Renderer();

        initEmulator();
        registerFilePickerLauncher();
        registerAddRomLauncher();
        initUI(); // Initialize UI components

        loadROM("/data/user/0/com.example.ggboy/files/ROMs/Pokemon_Gelbe_Edition.gb");
    }

    public native void initEmulator();

    public native void loadROM(String romPath);
    public native void setButtonState(int buttonID, boolean pressed);
}
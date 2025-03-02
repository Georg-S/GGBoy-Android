package com.example.ggboy;

import com.example.ggboy.FilePickerActivity;
import com.example.ggboy.Utility;
import androidx.activity.result.ActivityResultLauncher;
import androidx.appcompat.app.AppCompatActivity;

import android.net.Uri;
import androidx.activity.result.contract.ActivityResultContracts;

import android.os.FileUtils;
import android.view.Menu;
import android.view.MenuItem;
import android.os.Bundle;
import android.content.Intent;

import com.example.ggboy.databinding.ActivityMainBinding;

import java.io.FileNotFoundException;
import java.io.InputStream;


public class MainActivity extends AppCompatActivity
{
    public static String ROM_PATH = "ROMs";
    private ActivityMainBinding binding;
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
                        return;
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

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        setContentView(R.layout.activity_main);

        initEmulator();
        registerFilePickerLauncher();
        registerAddRomLauncher();
    }

    public native void initEmulator();
    public native void loadROM(String romPath);
}
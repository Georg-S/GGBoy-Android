package com.example.ggboy;

import android.content.Intent;
import android.os.Bundle;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.LinearLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class SelectSaveStateActivity extends AppCompatActivity
{
    public static final String EXTRA_SAVE_STATE_DIRECTORY = "EXTRA_SAVE_STATE_DIRECTORY";
    public static final String EXTRA_SAVE_STATE_THUMBNAIL_DIRECTORY = "EXTRA_SAVE_STATE_THUMBNAIL_DIRECTORY";
    public static final String EXTRA_IS_LOADING = "EXTRA_IS_LOADING";
    public static final String EXTRA_OUT_FILE_PATH = "EXTRA_OUT_FILE_PATH";
    public static final String EXTRA_OUT_THUMBNAIL_PATH = "EXTRA_OUT_THUMBNAIL_PATH";
    public static final int NO_FILES = 2;
    private final String baseFileName = "SaveState ";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_selectsavestate);

        String saveStateDirectory = getIntent().getStringExtra(EXTRA_SAVE_STATE_DIRECTORY);
        String saveStateThumbnailDirectory = getIntent().getStringExtra(EXTRA_SAVE_STATE_THUMBNAIL_DIRECTORY);
        boolean isLoading = getIntent().getBooleanExtra(EXTRA_IS_LOADING, false);

        File internalFilesDir = getFilesDir();
        var saveStateFiles = Utility.getFilesInDirectory(internalFilesDir + "/" + saveStateDirectory);

        if (saveStateDirectory == null && saveStateThumbnailDirectory == null)
        {
            int b = 3;
        }

        if (isLoading && saveStateFiles.isEmpty())
        {
            setResult(NO_FILES);
            finish();
            return;
        }

        RecyclerView recyclerView = findViewById(R.id.recyclerViewSaveStates);
        recyclerView.setLayoutManager(new LinearLayoutManager(this));

        List<SaveState> saveStates = new ArrayList<>();
        if (!isLoading)
        {
            for (int i = 0; i < 12; i++)
            {
                var saveStateFilename = baseFileName + Integer.toString(i);
                var saveStateFilePath = internalFilesDir + "/" + saveStateDirectory + "/" + saveStateFilename;
                var saveStateThumbnailPath = internalFilesDir + "/" + saveStateThumbnailDirectory + "/" + saveStateFilename;
                saveStates.add(new SaveState(saveStateFilename, saveStateFilePath, saveStateThumbnailPath));
            }
        }

        SaveStateAdapter adapter = new SaveStateAdapter(saveStates, saveState -> {
            Intent resultIntent = new Intent();
            resultIntent.putExtra(EXTRA_OUT_FILE_PATH, saveState.getFilePath());
            resultIntent.putExtra(EXTRA_OUT_THUMBNAIL_PATH, saveState.getThumbnailPath());
            setResult(RESULT_OK, resultIntent);
            finish(); // quit and return selection
        });
        recyclerView.setAdapter(adapter);
    }
}
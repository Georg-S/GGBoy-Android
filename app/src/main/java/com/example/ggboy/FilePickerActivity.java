package com.example.ggboy;

import android.content.Intent;
import android.os.Bundle;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import androidx.appcompat.app.AppCompatActivity;

import java.io.File;
import java.util.stream.Collectors;

public class FilePickerActivity extends AppCompatActivity
{
    public static final String EXTRA_DIRECTORY = "EXTRA_DIRECTORY";
    public static final int NO_FILES = 2;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_picker);

        ListView listView = findViewById(R.id.listView);

        String directoryPath = getIntent().getStringExtra(EXTRA_DIRECTORY);
        File internalFilesDir = getFilesDir();

        var files = Utility.getFilesInDirectory(internalFilesDir + "/" + directoryPath);
        var fileNames = files.stream().map(File::getName).collect(Collectors.toList());
        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, fileNames);
        listView.setAdapter(adapter);

        if (files.isEmpty())
        {
            setResult(NO_FILES);
            finish();
            return;
        }

        listView.setOnItemClickListener((parent, view, position, id) ->
        {
            String filePath = files.get(position).getAbsolutePath();
            Intent resultIntent = new Intent();
            resultIntent.putExtra("selectedFile", filePath);
            setResult(RESULT_OK, resultIntent);
            finish();
        });
    }
}

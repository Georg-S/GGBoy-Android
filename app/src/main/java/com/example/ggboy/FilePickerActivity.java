package com.example.ggboy;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public class FilePickerActivity extends Activity
{
    public static final String EXTRA_DIRECTORY = "EXTRA_DIRECTORY";
    private List<String> filePaths;
    private List<String> fileNames;

    @Override
    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_file_picker);

        ListView listView = findViewById(R.id.listView);

        String directoryPath = getIntent().getStringExtra(EXTRA_DIRECTORY);
        File internalFilesDir = getFilesDir();
        getFilesInDirectory( internalFilesDir + "/" + directoryPath);
        ArrayAdapter<String> adapter = new ArrayAdapter<>(this, android.R.layout.simple_list_item_1, fileNames);
        listView.setAdapter(adapter);

        if (fileNames.isEmpty())
        {
            setResult(Activity.RESULT_CANCELED);
            finish();
            return;
        }

        listView.setOnItemClickListener(new AdapterView.OnItemClickListener()
        {
            @Override
            public void onItemClick(AdapterView<?> parent, View view, int position, long id)
            {
                String filePath = filePaths.get(position);
                Intent resultIntent = new Intent();
                resultIntent.putExtra("selectedFile", filePath);
                setResult(RESULT_OK, resultIntent);
                finish();
            }
        });
    }

    private void getFilesInDirectory(String directoryPath)
    {
        filePaths = new ArrayList<String>();
        fileNames = new ArrayList<String>();

        File directory = new File(directoryPath);
        File[] files = directory.listFiles();
        if (files == null)
            return;

        for (File file : files)
        {
            filePaths.add(file.getAbsolutePath());
            fileNames.add(file.getName());
        }
    }
}

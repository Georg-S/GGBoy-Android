package com.example.ggboy;

import android.net.Uri;

public class SaveState
{
    private String fileName;
    private String filePath;
    private String thumbnailPath;

    public SaveState(String fileName, String filePath, String thumbnailPath)
    {
        this.fileName = fileName;
        this.filePath = filePath;
        this.thumbnailPath = thumbnailPath;
    }

    public String getFileName()
    {
        return fileName;
    }

    public String getFilePath()
    {
        return filePath;
    }

    public String getThumbnailPath()
    {
        return thumbnailPath;
    }

    public void setScreenshotPath(String thumbnailPath)
    {
        this.thumbnailPath = thumbnailPath;
    }
}

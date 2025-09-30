package com.example.ggboy;

import java.io.File;

import android.content.ContentResolver;
import android.content.Context;
import android.net.Uri;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.FileInputStream;
import java.io.InputStream;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Arrays;

import android.database.Cursor;
import android.provider.OpenableColumns;

public class Utility
{
    public static String getFileName(Context context, Uri uri)
    {
        String result = null;
        if (uri.getScheme().equals("content"))
        {
            Cursor cursor = context.getContentResolver().query(uri, null, null, null, null);
            try
            {
                if (cursor != null && cursor.moveToFirst())
                {
                    int nameIndex = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME);
                    if (nameIndex != -1)
                        result = cursor.getString(nameIndex);
                }
            } finally
            {
                cursor.close();
            }
        }
        if (result == null) {
            result = uri.getPath();
            int cut = result.lastIndexOf('/');
            if (cut != -1) {
                result = result.substring(cut + 1);
            }
        }
        return result;
    }


    public static void createInternalFolder(Context context, String folderName)
    {
        File romFolder = new File(context.getFilesDir(), folderName);
        if (!romFolder.exists())
            romFolder.mkdirs();
    }

    public static boolean copyFileToInternalFolder(Context context, String folderName, Uri fileUri)
    {
        InputStream inputStream = null;
        FileOutputStream outputStream = null;
        try
        {
            String fileName = getFileName(context, fileUri);

            createInternalFolder(context, folderName);

            ContentResolver contentResolver = context.getContentResolver();
            inputStream = contentResolver.openInputStream(fileUri);

            // Open a FileOutputStream to the desired file in the "ROM" folder
            File file = new File(context.getFilesDir() + "/" + folderName, fileName);
            outputStream = new FileOutputStream(file);

            // Write the data from InputStream to the FileOutputStream
            byte[] buffer = new byte[1024];
            int bytesRead;
            while ((bytesRead = inputStream.read(buffer)) != -1)
            {
                outputStream.write(buffer, 0, bytesRead);
            }
        }
        catch (IOException e)
        {
            e.printStackTrace();
            return false;
        }
        finally
        {
            try
            {
                if (inputStream != null)
                    inputStream.close();
                if (outputStream != null)
                    outputStream.close();
            }
            catch (IOException e)
            {
                e.printStackTrace();
                return false;
            }
        }
        return true;
    }

    public static ArrayList<File> getFilesInDirectory(String directoryPath)
    {
        var result = new ArrayList<File>();

        File directory = new File(directoryPath);
        File[] files = directory.listFiles();
        if (files == null)
            return result;

        result.addAll(Arrays.asList(files));

        return result;
    }
}

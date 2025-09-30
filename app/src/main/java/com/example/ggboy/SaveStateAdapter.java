package com.example.ggboy;

import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.recyclerview.widget.RecyclerView;

import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.List;

public class SaveStateAdapter extends RecyclerView.Adapter<SaveStateAdapter.SaveStateViewHolder> {

    public interface OnItemClickListener {
        void onItemClick(SaveState saveState);
    }

    private List<SaveState> saveStateList;
    private OnItemClickListener listener;

    public SaveStateAdapter(List<SaveState> saveStateList, OnItemClickListener listener) {
        this.saveStateList = saveStateList;
        this.listener = listener;
    }

    public static class SaveStateViewHolder extends RecyclerView.ViewHolder {
        ImageView imageScreenshot;
        TextView textFileName;

        public SaveStateViewHolder(View itemView) {
            super(itemView);
            imageScreenshot = itemView.findViewById(R.id.imageScreenshot);
            textFileName = itemView.findViewById(R.id.textFileName);
        }

        public void bind(SaveState saveState, OnItemClickListener listener) {
            textFileName.setText(saveState.getFileName());
            itemView.setOnClickListener(v -> listener.onItemClick(saveState));

            Path path = Paths.get(saveState.getThumbnailPath());
            if (Files.exists(path) && Files.isRegularFile(path))
            {
                try
                {
                    byte[] data = Files.readAllBytes(path);
                    var bitmap = Renderer.createBitMapFromEmulatorBinaryImage(data);
                    imageScreenshot.setImageBitmap(bitmap);
                } catch (IOException e)
                {
                    // TODO logging?
                }
            }
        }
    }

    @NonNull
    @Override
    public SaveStateViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        View view = LayoutInflater.from(parent.getContext())
                .inflate(R.layout.item_savestate, parent, false);
        return new SaveStateViewHolder(view);
    }

    @Override
    public void onBindViewHolder(SaveStateViewHolder holder, int position) {
        holder.bind(saveStateList.get(position), listener);
    }

    @Override
    public int getItemCount() {
        return saveStateList.size();
    }
}


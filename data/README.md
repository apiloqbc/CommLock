# 📁 Data Folder

This folder contains video and audio files for the ESP32-S3 Video Player project.

## 🎬 Video Files

Place your MJPEG video files here:

- `home.mjpeg` - Home video (plays automatically on startup)
- `video_1.mjpeg` - Video for button 1
- `video_2.mjpeg` - Video for button 2
- `video_3.mjpeg` - Video for button 3
- `video_4.mjpeg` - Video for button 4
- `video_5.mjpeg` - Video for button 5
- `video_6.mjpeg` - Video for button 6
- `video_7.mjpeg` - Video for button 7
- `video_8.mjpeg` - Video for button 8
- `video_9.mjpeg` - Video for button 9

## 🎵 Audio Files

Audio files are stored on the MicroSD card in the DFPlayer Mini module:

- `0001.mp3` - Audio for video 1
- `0002.mp3` - Audio for video 2
- `0003.mp3` - Audio for video 3
- ... and so on

## 📋 File Requirements

### Video Files:
- **Format:** MJPEG
- **Resolution:** 128x160 (recommended)
- **Duration:** 5-10 seconds (optimal)
- **Size:** Keep under 1MB for smooth playback

### Audio Files:
- **Format:** MP3
- **Bitrate:** 128kbps (recommended)
- **Duration:** Match video duration
- **Naming:** 0001.mp3, 0002.mp3, etc.

## 🔧 Git LFS

This project uses Git LFS (Large File Storage) to handle video files efficiently:

### Adding New Video Files:
```bash
# Add video file to Git LFS
git add data/video_1.mjpeg
git commit -m "Add video file via Git LFS"
```

### Checking LFS Status:
```bash
# Check which files are tracked by LFS
git lfs ls-files

# Check LFS status
git lfs status
```

### Converting Existing Files:
```bash
# Convert existing files to LFS
git lfs migrate import --include="data/*.mjpeg"
```

## 📊 File Size Guidelines

- **Small videos (< 500KB):** Ideal for smooth playback
- **Medium videos (500KB-1MB):** Acceptable, may have slight delays
- **Large videos (> 1MB):** Not recommended, may cause memory issues

## 🎯 Best Practices

1. **Optimize videos** before adding to the project
2. **Test playback** on actual hardware
3. **Keep file sizes small** for better performance
4. **Use consistent naming** for easy management
5. **Backup original files** before conversion 
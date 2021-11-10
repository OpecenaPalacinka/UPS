import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.InputStream;

public class ImageLoader {

    String[] names = {"pogchamp.png","trump.png","monkaH.png","putin.png"};
    int[] widths = {135,125,160,155};
    int[] heights = {150,150,135,150};

    int pogchampWidth;
    int pogchampHeight;

    int trumpWidth;
    int trumpHeight;

    int putinWidth;
    int putinHeight;

    int monkaHWidth;
    int monkaHHeight;

    public ImageView[] loadImages(int num) throws FileNotFoundException {
        ImageView[] images = new ImageView[num];
        ImageView imageView;
        for (int i = 0;i < num; i++){
            imageView = loadImage(names[i]);
            imageView.setFitWidth(widths[i]);
            imageView.setFitHeight(heights[i]);
            images[i] = imageView;
        }

        return images;
    }

    public ImageView loadImage(String name) throws FileNotFoundException {
        InputStream inputStream = new FileInputStream("../"+name);
        Image image = new Image(inputStream);
        ImageView imageView = new ImageView();
        imageView.setImage(image);
        return imageView;
    }
}

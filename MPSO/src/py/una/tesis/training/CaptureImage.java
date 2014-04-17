/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package py.una.tesis.training;

/**
 *
 * @author lg_more
 */
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.MatOfRect;
import org.opencv.core.Point;
import org.opencv.core.Rect;
import org.opencv.core.Scalar;
import org.opencv.highgui.Highgui;
import static org.opencv.highgui.Highgui.CV_LOAD_IMAGE_GRAYSCALE;
import static org.opencv.highgui.Highgui.imread;
import static org.opencv.highgui.Highgui.imwrite;
import org.opencv.imgproc.CLAHE;
import org.opencv.objdetect.CascadeClassifier;

public class CaptureImage extends CLAHE {

    
    public CaptureImage(){
        super(new Long(1));
        
    }
    

    public static void main(String[] args) throws InstantiationException, IllegalAccessException {
        System.loadLibrary(Core.NATIVE_LIBRARY_NAME);
        System.out.println(Core.VERSION);
        Mat m= imread("Firefox_wallpaper.png",CV_LOAD_IMAGE_GRAYSCALE); //input image
        Mat salida = null;
        
       
        //imwrite("salida.png",salida);
        System.out.println("m = " + m.dump());
    }

}

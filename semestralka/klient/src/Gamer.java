import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;

import java.io.IOException;

public class Gamer {


    IntegerProperty fingersSelected = new SimpleIntegerProperty(0);

    public int getFingersSelected() {
        return fingersSelected.get();
    }

    public IntegerProperty fingersSelectedProperty() {
        return fingersSelected;
    }

    public void setFingersSelected(int fingersSelected) {
        this.fingersSelected.set(fingersSelected);
    }

    public Gamer() {

    }


}

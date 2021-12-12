import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;

import java.io.IOException;

public class Gamer {

    String name = "";

    public int getNumOfFingers() {
        return numOfFingers;
    }

    public void setNumOfFingers(int numOfFingers) {
        this.numOfFingers = numOfFingers;
    }

    int numOfFingers = 2;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

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

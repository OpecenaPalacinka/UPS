import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;

import java.io.IOException;

public class Gamer {


    String name;

    Message message;

    IntegerProperty fingersSelected = new SimpleIntegerProperty(0);

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public int getFingersSelected() {
        return fingersSelected.get();
    }

    public IntegerProperty fingersSelectedProperty() {
        return fingersSelected;
    }

    public void setFingersSelected(int fingersSelected) {
        this.fingersSelected.set(fingersSelected);
    }

    public Gamer(String name) {
        this.name = name;
        message = new Message();
    }


}

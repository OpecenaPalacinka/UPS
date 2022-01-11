import javafx.beans.property.IntegerProperty;
import javafx.beans.property.SimpleIntegerProperty;

public class Gamer {

    /**
     * Jméno hráče
     */
    String name = "";

    /**
     * Počet prstů
     */
    int numOfFingers = 2;

    /**
     * Vrací počet prstů hráče
     * @return počet prstů
     */
    public int getNumOfFingers() {
        return numOfFingers;
    }

    /**
     * Nastavuje počet prstů hráče
     * @param numOfFingers počet prstů hráče
     */
    public void setNumOfFingers(int numOfFingers) {
        this.numOfFingers = numOfFingers;
    }

    /**
     * Vrací jméno hráče
     * @return jméno hráče
     */
    public String getName() {
        return name;
    }

    /**
     * Nastavuje jméno hráče
     * @param name jméno
     */
    public void setName(String name) {
        this.name = name;
    }

    ///////// Stejné jako numOfFingers, akorát IntegerProperty, potřeba kvůli JavaFX
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
    //////////


    public Gamer() {

    }


}

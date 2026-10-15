public class Calc_BMI {

    public static void main(String[] args) {
        double height = 1.7,
               weight = 65.0;
        double BMI = weight/(height*height);
        System.out.println(BMI);
    }
}
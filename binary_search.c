#include <stdio.h>

int binarySearch(double arr[], int n, double x) {
   int l = 0, r = n - 1;
   while (l <= r) {
      int mid = l + (r - l) / 2;
      if (arr[mid] == x)
         return mid;
      if (arr[mid] < x)
         l = mid + 1;
      else
         r = mid - 1;
   }
   return -1;
}

int main(void) {
    double array_p_nerr[220] = {0.729, 0.7371, 0.7452, 0.74529, 0.7533, 0.75348, 0.753571, 0.7614, 0.76167, 0.76176, 0.761852, 0.7695, 0.76986, 0.77004, 0.770133, 0.770224, 0.7776, 0.77805, 0.77832, 0.77841, 0.778414, 0.778596, 0.778688, 0.7857, 0.78624, 0.7866, 0.786695, 0.78678, 0.786968, 0.787059, 0.787152, 0.7938, 0.79443, 0.79488, 0.794976, 0.79515, 0.79524, 0.79534, 0.795522, 0.795616, 0.795708, 0.8019, 0.80262, 0.80316, 0.803257, 0.80352, 0.8037, 0.803712, 0.803985, 0.804076, 0.80408, 0.804264, 0.804357, 0.81081, 0.81144, 0.811538, 0.81189, 0.812084, 0.81216, 0.81225, 0.812448, 0.812544, 0.81263, 0.81282, 0.812912, 0.813006, 0.81972, 0.819819, 0.82026, 0.820456, 0.82062, 0.8208, 0.820911, 0.821008, 0.821184, 0.821275, 0.821376, 0.82156, 0.821655, 0.821748, 0.82863, 0.828828, 0.82908, 0.82935, 0.829374, 0.82944, 0.829472, 0.829738, 0.82992, 0.829932, 0.830208, 0.8303, 0.830304, 0.83049, 0.830584, 0.83754, 0.837837, 0.8379, 0.837936, 0.83808, 0.838292, 0.838488, 0.838565, 0.838656, 0.838856, 0.838953, 0.83904, 0.839232, 0.839325, 0.83942, 0.84645, 0.84672, 0.84681, 0.846846, 0.847044, 0.84721, 0.847392, 0.847504, 0.847602, 0.84778, 0.847872, 0.847974, 0.84816, 0.848256, 0.84835, 0.85536, 0.85554, 0.855855, 0.856128, 0.856152, 0.856219, 0.856251, 0.85652, 0.856704, 0.856716, 0.856995, 0.857088, 0.857092, 0.85728, 0.857375, 0.86427, 0.86436, 0.864864, 0.865046, 0.86526, 0.865458, 0.865536, 0.865628, 0.86583, 0.865928, 0.866016, 0.86621, 0.866304, 0.8664, 0.87318, 0.873873, 0.873964, 0.874368, 0.874552, 0.874665, 0.874764, 0.874944, 0.875037, 0.87514, 0.875328, 0.875425, 0.87552, 0.88209, 0.882882, 0.883476, 0.883568, 0.883872, 0.884058, 0.88407, 0.884352, 0.884446, 0.88445, 0.88464, 0.884736, 0.891891, 0.892584, 0.893079, 0.893172, 0.893376, 0.893475, 0.893564, 0.89376, 0.893855, 0.893952, 0.901692, 0.902286, 0.902682, 0.902776, 0.90288, 0.90307, 0.903168, 0.903264, 0.911493, 0.911988, 0.912285, 0.91238, 0.912384, 0.912576, 0.912673, 0.921294, 0.92169, 0.921888, 0.921984, 0.922082, 0.931095, 0.931392, 0.931491, 0.931588, 0.940896, 0.941094, 0.941192, 0.950697, 0.950796, 0.960498, 0.970299, };
    for (int i = 0; i < 220; i++) {
    int result = binarySearch(array_p_nerr, 220, array_p_nerr[i]);
    printf("%d\n", result);
   }
   return 0;
}

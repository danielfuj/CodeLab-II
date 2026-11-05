#include <algorithm>
#include <array>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

using namespace std;

// Structure to hold journey emission details
struct EmissionRecord {
    string transport;
    double distanceKm = 0.0;
    double energyUsed = 0.0;
    double factor = 0.0;
    double carbonKg = 0.0;
};

class CarbonCalculator {
private:
    // Standard carbon emission factors
    const double petrolFactor = 2.31;
    const double dieselFactor = 2.68;
    const double gridElectricityFactor = 0.207;

    // Helper to read text input from user
    string readText(const string& prompt) const {
        cout << prompt;
        string value;
        getline(cin, value);
        return value;
    }

    // Helper to read and validate numeric inputs within a range
    double readNumber(const string& prompt, double minimum, double maximum) const {
        while (true) {
            const string input = readText(prompt);
            try {
                size_t used = 0;
                const double value = stod(input, &used);
                if (used == input.size() && isfinite(value) &&
                    value >= minimum && value <= maximum)
                    return value;
            } catch (...) {
            }
            cout << "Enter a valid value between " << minimum
                 << " and " << maximum << ".\n";
        }
    }

    // Helper to read menu option choices safely
    int readChoice(const string& prompt, int minimum, int maximum) const {
        while (true) {
            const string input = readText(prompt);
            try {
                size_t used = 0;
                const int value = stoi(input, &used);
                if (used == input.size() && value >= minimum && value <= maximum)
                    return value;
            } catch (...) {
            }
            cout << "Select a valid option.\n";
        }
    }

    // Calculates carbon emissions for a specific journey type
    EmissionRecord calculate(int type, double distance, double consumption) const {
        EmissionRecord result;
        result.distanceKm = distance;

        if (type == 1) {
            result.transport = "Petrol car";
            result.energyUsed = distance * consumption / 100.0;
            result.factor = petrolFactor;
        } else if (type == 2) {
            result.transport = "Diesel car";
            result.energyUsed = distance * consumption / 100.0;
            result.factor = dieselFactor;
        } else if (type == 3) {
            result.transport = "Electric car";
            result.energyUsed = distance * consumption / 100.0;
            result.factor = gridElectricityFactor;
        } else if (type == 4) {
            result.transport = "Walking";
        } else {
            result.transport = "Cycling";
        }

        result.carbonKg = result.energyUsed * result.factor;
        return result;
    }

    // Displays transport selection menu and returns choice
    int selectTransport() const {
        cout << "\n1. Petrol car\n"
             << "2. Diesel car\n"
             << "3. Electric car\n"
             << "4. Walking\n"
             << "5. Cycling\n";
        return readChoice("Select transport: ", 1, 5);
    }

    // Collects inputs for a single journey
    EmissionRecord collectJourney() const {
        const int type = selectTransport();
        const double distance = readNumber("Journey distance in kilometres: ", 0.01, 1000000.0);
        double consumption = 0.0;

        if (type == 1 || type == 2)
            consumption = readNumber("Fuel consumption (litres per 100 km): ", 0.01, 100.0);
        else if (type == 3)
            consumption = readNumber("Electricity consumption (kWh per 100 km): ", 0.01, 200.0);

        return calculate(type, distance, consumption);
    }

    // Outputs emission results in a formatted block
    void display(const EmissionRecord& result) const {
        cout << fixed << setprecision(2)
             << "\n========== EMISSIONS RESULT ==========\n"
             << "Transport: " << result.transport << '\n'
             << "Distance: " << result.distanceKm << " km\n"
             << "Fuel/electricity used: " << result.energyUsed << '\n'
             << "Emission factor: " << result.factor << '\n'
             << "Estimated CO2: " << result.carbonKg << " kg\n"
             << "======================================\n";
    }

    // Runs single journey estimation
    void singleJourney() const {
        display(collectJourney());
        cout << "This estimate covers operational emissions only.\n";
    }

    // Compares multiple transport modes for a given distance
    void compareOptions() const {
        const double distance = readNumber("Common journey distance (km): ", 0.01, 1000000.0);
        const double petrol = readNumber("Petrol use (litres per 100 km): ", 0.01, 100.0);
        const double diesel = readNumber("Diesel use (litres per 100 km): ", 0.01, 100.0);
        const double electric = readNumber("Electricity use (kWh per 100 km): ", 0.01, 200.0);

        const vector<EmissionRecord> results = {
            calculate(1, distance, petrol),
            calculate(2, distance, diesel),
            calculate(3, distance, electric),
            calculate(4, distance, 0.0),
            calculate(5, distance, 0.0)
        };

        cout << "\n" << left << setw(20) << "Transport"
             << right << setw(18) << "CO2 (kg)\n"
             << string(38, '-') << '\n';

        for (const auto& result : results)
            cout << left << setw(20) << result.transport
                 << right << setw(18) << fixed << setprecision(2)
                 << result.carbonKg << '\n';

        const auto best = min_element(results.begin(), results.end(),
            [](const auto& a, const auto& b) {
                return a.carbonKg < b.carbonKg;
            });

        cout << "\nLowest estimated operational emissions: "
             << best->transport << ".\n";
    }

    // Projects annual emissions based on weekly trip frequency
    void annualProjection() const {
        const EmissionRecord journey = collectJourney();
        const int weeklyTrips = readChoice("Similar journeys per week: ", 1, 500);
        const double annual = journey.carbonKg * weeklyTrips * 52.0;

        cout << fixed << setprecision(2)
             << "\n========== ANNUAL PROJECTION ==========\n"
             << "Emissions per journey: " << journey.carbonKg << " kg\n"
             << "Journeys per year: " << weeklyTrips * 52 << '\n'
             << "Annual emissions: " << annual << " kg CO2\n"
             << "Annual emissions: " << annual / 1000.0 << " tonnes CO2\n";
    }

    // Calculates target emission reductions based on user targets
    void reductionTarget() const {
        const double current = readNumber("Current annual emissions (kg CO2): ", 0.01, 100000000.0);
        const double targetPercent = readNumber("Target reduction percentage: ", 1.0, 100.0);
        const double reduction = current * targetPercent / 100.0;
        const double target = current - reduction;

        cout << fixed << setprecision(2)
             << "\n========== REDUCTION TARGET ==========\n"
             << "Current emissions: " << current << " kg CO2\n"
             << "Target reduction: " << reduction << " kg CO2\n"
             << "Target annual emissions: " << target << " kg CO2\n";
    }

public:
    // Main application menu loop
    void run() const {
        while (true) {
            cout << "\n========== CO2 CALCULATOR ==========\n"
                 << "1. Calculate a journey\n"
                 << "2. Compare transport options\n"
                 << "3. Estimate annual emissions\n"
                 << "4. Set a reduction target\n"
                 << "5. Exit\n";

            const int choice = readChoice("Select an option: ", 1, 5);

            switch (choice) {
                case 1: singleJourney(); break;
                case 2: compareOptions(); break;
                case 3: annualProjection(); break;
                case 4: reductionTarget(); break;
                case 5: return;
            }
        }
    }
};

int main() {
    CarbonCalculator application;
    application.run();
    return 0;
}
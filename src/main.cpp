#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <stdexcept>
#include <typeinfo>

struct dataset {
    std::vector<float> longitude;
    std::vector<float> latitude;
    std::vector<float> housing_median_age;
    std::vector<float> total_rooms;
    std::vector<float> total_bedrooms;
    std::vector<float> population;
    std::vector<float> households;
    std::vector<float> median_income;
    std::vector<float> median_house_value;
    std::vector<float> ocean_proximity;

    std::vector<float>& operator[](const size_t index){
        if (index == 0){return longitude;}
        else if (index == 1){return latitude;}
        else if (index == 2){return housing_median_age;}
        else if (index == 3){return total_rooms;}
        else if (index == 4){return total_bedrooms;}
        else if (index == 5){return population;}
        else if (index == 6){return households;}
        else if (index == 7){return median_income;}
        else if (index == 8){return median_house_value;}
        else if (index == 9){return ocean_proximity;}
        else{throw std::out_of_range("No ojects by this index");}
    }
};

void min_max_foo(dataset& dt, std::vector<float>& min_max_values){
    float min_value = dt[0][0], max_value = dt[0][0];
    for (size_t col = 0; col < 10; col++){
        for (size_t row = 0; row < dt[0].size(); row++){
            if (min_value > dt[col][row]){
                min_value = dt[col][row];
            }
            if (max_value < dt[col][row]){
                max_value = dt[col][row];
            }
        }
    }
    
    min_max_values.push_back(min_value);
    min_max_values.push_back(max_value);
    min_max_values.push_back(max_value - min_value);
    float min_max_value = max_value - min_value;

    for (size_t col = 0; col < 10; col++){
        for (size_t row = 0; row < dt[0].size(); row++){
            dt[col][row] = (dt[col][row] - min_value) / min_max_value;
        }
    }
}


void csv_parser(dataset& dt, const std::string file_location){
    std::ifstream file(file_location);
    std::vector<std::string> raw_data;
    std::string raw_line;
    if (file.is_open()){
        std::cout << "File was found." << std::endl;
        while (getline(file, raw_line)){
            raw_data.push_back(raw_line);
        }

        size_t comma_counter;
        bool flag;
        for (size_t raw_data_index = 0; raw_data_index < raw_data.size(); raw_data_index++){
            comma_counter = 0;
            flag = true;
            for (size_t symbol_index = 0; symbol_index < raw_data[raw_data_index].length()-1; symbol_index++){
                if (raw_data[raw_data_index][symbol_index] == raw_data[raw_data_index][symbol_index+1] && raw_data[raw_data_index][symbol_index] == ','){
                    flag = false;
                }
            }
            if (flag){
                for (size_t symbol_index = 0; symbol_index < raw_data[raw_data_index].length(); symbol_index++){
                    if (raw_data[raw_data_index][symbol_index] != ','){
                        raw_line += raw_data[raw_data_index][symbol_index];
                    }
                    else{
                        dt[comma_counter].push_back(std::stof(raw_line));
                        raw_line.clear();
                        comma_counter++;
                    }

                    if (symbol_index == raw_data[raw_data_index].length()-1){
                        dt[comma_counter].push_back(std::stof(raw_line));
                        raw_line.clear();
                    }
                }
            }
        }
    }
    else{
        std::cout << "No such file direction..." << std::endl;
    }
    file.close();
}

float decryption_foo(float number, const std::vector<float> min_max){
    return (number * min_max[2]) + min_max[0];
}

float layer(const std::vector<float>& inputs,const std::vector<float>& weights){
    float result;
    for (size_t iter = 0; iter < weights.size(); iter++){
        result += inputs[iter] * weights[iter];
    }
    return result;
}

void update_weights(const float result, const float correct_answer, const std::vector<float>& inputs, std::vector<float>& weights, const float learning_rate = 0.001){
    for (size_t i = 0; i < weights.size(); i++){
        weights[i] = weights[i] - learning_rate * (result - correct_answer) * inputs[i];
    }
}

int main(){
    dataset dt;
    std::vector<float> min_max_values;

    csv_parser(dt, "data/housing.csv");
    min_max_foo(dt, min_max_values);

    std::vector<float> weights;
    for (size_t i = 0; i < 9; i++){
        weights.push_back(0.5);
    }
    std::vector<float> inputs;

    float result;
    const int epochs = 2000;
    for (int eps = 0; eps < epochs; eps++){
        for (size_t i = 0; i < 20000; i++){
            inputs = {dt[0][i], dt[1][i], dt[2][i], dt[3][i], dt[4][i], dt[5][i], dt[6][i], dt[7][i], dt[9][i]};
            result = layer(inputs, weights);
            update_weights(result, dt[8][i], inputs, weights);
        }
    }

    std::vector<float> loses;
    float procent_loss;
    int test_id = 20000;

    for (size_t test_id = 20000; test_id < dt[0].size(); test_id++){
        inputs = {dt[0][test_id], dt[1][test_id], dt[2][test_id], dt[3][test_id], dt[4][test_id], dt[5][test_id], dt[6][test_id], dt[7][test_id], dt[9][test_id]};
        procent_loss = ((decryption_foo(layer(inputs, weights), min_max_values) * 100.f) / decryption_foo(dt[8][test_id], min_max_values)) - 100;
        std::cout << "Loss is " << procent_loss << '%' << std::endl;
        loses.push_back(procent_loss);
    }

    float loss_sum;
    for (size_t i = 0; i < loses.size(); i++){
        loss_sum += loses[i];
    }
    std::cout << '\n' << std::endl;
    loss_sum /= loses.size();
    std::cout << "Avg loss = " << loss_sum << std::endl;
    return 0;
}
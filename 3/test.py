#!/usr/bin/env python3
"""
Testing script for the tempstats C extension module.
"""

import tempstats
import random
import statistics

def test_basic_functionality():
    """Test with basic temperature data"""
    print("!!!!! Basic Functionality Test !!!!!")
    
    #Hardcoded temps in a list.
    temperatures = [20.5, 21.3, 19.8, 22.1, 20.9, 18.7, 23.4, 21.0]
    
    print(f"Temperature readings: {temperatures}")
    print(f"Count: {tempstats.count_readings(temperatures)}")
    print(f"Min temperature: {tempstats.min_temp(temperatures):.2f}°C")
    print(f"Max temperature: {tempstats.max_temp(temperatures):.2f}°C")
    print(f"Average temperature: {tempstats.avg_temp(temperatures):.2f}°C")
    print(f"Variance: {tempstats.variance_temp(temperatures):.4f}")
    
    #Python has a statistics module we can use!
    print("\n!!!!! Verification with Python statistics module !!!!!")
    print(f"Python min: {min(temperatures):.2f}°C")
    print(f"Python max: {max(temperatures):.2f}°C")
    print(f"Python mean: {statistics.mean(temperatures):.2f}°C")
    print(f"Python variance (sample): {statistics.variance(temperatures):.4f}")

def test_edge_cases():
    print("\n!!!!! Edge Cases Test !!!!!")
    
    #One singular reading
    single_temp = [25.5]
    print(f"Single reading: {single_temp}")
    print(f"Count: {tempstats.count_readings(single_temp)}")
    print(f"Min: {tempstats.min_temp(single_temp):.2f}°C")
    print(f"Max: {tempstats.max_temp(single_temp):.2f}°C")
    print(f"Average: {tempstats.avg_temp(single_temp):.2f}°C")
    print(f"Variance (should be 0): {tempstats.variance_temp(single_temp):.4f}")
    
    #Testing with empty list shouldn't be accepted.
    print("\nTesting empty list (should show error):")
    try:
        tempstats.min_temp([])
    except Exception as e:
        print(f"Expected error: {e}")

def test_real_time_simulation():
    """Simulate real-time data processing scenario"""
    print("\n!!!!! Real-time Monitoring Simulation !!!!!")
    
    #Simulate real time data readings every 10 seconds.
    sensor_readings = []
    
    print("Simulating sensor readings...")
    for second in range(10):
        #Some noise on temperature
        base_temp = 22.0
        noise = (random.random() - 0.5) * 2.0  
        current_temp = base_temp + noise
        
        sensor_readings.append(current_temp)
        print(f"Second {second + 1}: {current_temp:.2f}°C")
        
        if len(sensor_readings) > 1: 
            print(f"  Current stats - Min: {tempstats.min_temp(sensor_readings):.2f}°C, "
                  f"Max: {tempstats.max_temp(sensor_readings):.2f}°C, "
                  f"Avg: {tempstats.avg_temp(sensor_readings):.2f}°C")

if __name__ == "__main__":
    print("Temperature Statistics C Extension Test")
    print("=" * 50)
    
    test_basic_functionality()
    test_edge_cases()
    test_real_time_simulation()
    
    print("\nAll tests completed! Success!")
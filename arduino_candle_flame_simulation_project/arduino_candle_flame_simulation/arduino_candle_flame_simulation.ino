/*
 * Project: Realistic Physics-Based RGB Candle
 * Description: Simulates a realistic candle flame using smooth interpolation (Lerp),
 * Smoothstep easing, and Delta Time for fluid, non-blocking animation.
 * Includes a "Wind" feature controlled by a potentiometer.
 * * Hardware: Arduino Uno, RGB LED (Common Cathode), Potentiometer.
 */

// Pin Definitions
constexpr int POT_PIN {A1};    // Potentiometer input for "Wind Level"
constexpr int SEED_PIN {A0};   // Unconnected pin used for random noise generation

/**
 * Calculates the time elapsed since the last function call.
 * This ensures the animation runs at the same speed regardless of 
 * how fast the microcontroller is looping.
 * float Time in seconds (e.g., 0.016 for ~60fps).
 */
float calculateDeltaTime()
{
  static unsigned long previous_time_micros {};
  unsigned long current_time_micros {micros()};
  
  // Convert microseconds to seconds
  float delta_time {static_cast<float>(current_time_micros - previous_time_micros) / 1000000.0f};
  
  previous_time_micros = current_time_micros;
  return delta_time;
}

/**
 * Applies a sigmoid-like interpolation curve.
 * Converts a linear value (0.0 to 1.0) into an "S-curve" to make 
 * movement start slow, speed up, and slow down again.
 */
float smoothStep(float weight)
{
  if (weight > 1.0f) weight = 1.0f;
  else if (weight < 0.0f) weight = 0.0f;
  return 3.0f * weight * weight - 2.0f * weight * weight * weight;
}

/*
 * Linear Interpolation.
 * Finds a value between 'start' and 'end' based on 'weight' (0.0 to 1.0).
 */
float lerp(float weight, float start, float end)
{
  if (weight > 1.0f) weight = 1.0f;
  else if (weight < 0.0f) weight = 0.0f;
  return start * (1.0f - weight) + end * weight;
}

class Candle
{
private:
  // --- Hardware Configuration ---
  const int R_PIN;
  const int G_PIN;
  const int B_PIN;

  // --- Color Palette (RGB) ---
  const int hot_R  {255}; 
  const int hot_G  {100}; 
  const int hot_B  {20};  

  const int cold_R {255}; 
  const int cold_G {60};  
  const int cold_B {0};   

  // --- Animation Physics ---
  const float TRANSITION_SPEED_MIN {4.0f};
  const float TRANSITION_SPEED_MAX {7.0f};

  float counter {0.0f}; // Progress of current transition (0.0 to 1.0)
  float speed {TRANSITION_SPEED_MIN};

  // --- Brightness Limits ---
  const float lit {1.0f};
  const float dim {0.2f};

  // --- Current State Targets ---
  int start_r {}, target_r {};
  int start_g {}, target_g {};
  int start_b {}, target_b {};
  float start_brightness {}, target_brightness {};

public:
  // Constructor
  Candle(int r_pin, int g_pin, int b_pin) 
    : R_PIN {r_pin}, G_PIN {g_pin}, B_PIN {b_pin}
  {
  }

  /**
   * Sets up pin modes and seeds the random generator.
   * Call this inside setup().
   */
  void initialize()
  {
    // Important: Read analog noise to generate a true random seed
    randomSeed(analogRead(SEED_PIN));

    pinMode(R_PIN, OUTPUT);
    pinMode(G_PIN, OUTPUT);
    pinMode(B_PIN, OUTPUT);
  }

  /**
   * Main animation tick. Updates the LED color based on physics.
   * windLevel 0-100 value (higher = more chaotic flickering)
   * delta_time Time elapsed since last frame
   */
  void litUp(int windLevel, float delta_time)
  {
    if (delta_time <= 0.0f) return;

    // Advance the animation timer
    counter += speed * delta_time;

    // If the transition is finished, pick a new target color
    if (counter > 1.0f)
    {
      counter -= 1.0f; // Wrap around, keeping the overflow for smoothness
      pickNewTargets(windLevel);
    }

    // Calculate the smoothed weight for organic movement
    float smoothWeight {smoothStep(counter)};

    // Interpolate RGB values
    int current_r {static_cast<int>(lerp(smoothWeight, start_r, target_r))};
    int current_g {static_cast<int>(lerp(smoothWeight, start_g, target_g))};
    int current_b {static_cast<int>(lerp(smoothWeight, start_b, target_b))};
    
    // Interpolate Brightness
    float current_brightness {lerp(smoothWeight, start_brightness, target_brightness)};

    // Apply brightness and constrain to valid PWM range (0-255)
    int out_r {constrain(static_cast<int>(current_r * current_brightness), 0, 255)};
    int out_g {constrain(static_cast<int>(current_g * current_brightness), 0, 255)};
    int out_b {constrain(static_cast<int>(current_b * current_brightness), 0, 255)};
 
    // Output to hardware
    analogWrite(R_PIN, out_r);
    analogWrite(G_PIN, out_g);
    analogWrite(B_PIN, out_b);
  }

private:
  /*
   * Selects the next target color and brightness.
   * * Higher wind levels increase the probability of selecting 
   * lower brightness (dimming) and faster speeds (flickering).
   */
  void pickNewTargets(int windLevel)
  {
    // Determine if the flame should flicker based on wind intensity
    // map() ensures windLevel is within 0-100 bounds
    bool high_wind_event {random(0, 101) > (100 - constrain(windLevel, 0, 100))};
    
    // Select where on the interpolation curve to target (0.0 = Cold/Dim, 1.0 = Hot/Lit)
    // If high wind, bias towards the lower end (0.0 - 0.70)
    float selectWeight {(high_wind_event ? random(0, 70) : random(70, 101)) / 100.0f};

    // Update Red
    start_r = target_r;
    target_r = lerp(selectWeight, cold_R, hot_R);

    // Update Green
    start_g = target_g;
    target_g = lerp(selectWeight, cold_G, hot_G);

    // Update Blue
    start_b = target_b;
    target_b = lerp(selectWeight, cold_B, hot_B);

    // Update Brightness
    start_brightness = target_brightness;
    target_brightness = lerp(selectWeight, dim, lit);

    // Update Speed (High wind = Faster transitions)
    speed = lerp(selectWeight, TRANSITION_SPEED_MIN, TRANSITION_SPEED_MAX);
  }
};

// Instantiate the candle object on PWM pins 9, 10, 11
Candle candle {9, 10, 11}; 

void setup() 
{
  candle.initialize();
}

void loop() 
{
  // 1. Calculate the time step
  float delta_time {calculateDeltaTime()};

  // 2. Read sensors (Potentiometer)
  int potValue {analogRead(POT_PIN)};
  int windLevel {map(potValue, 0, 1023, 0, 100)};

  // 3. Update the simulation
  candle.litUp(windLevel, delta_time);
}
#ifndef LOW_PASS_FILTER_H
#define LOW_PASS_FILTER_H

class ExponentialSmoothing
{
  private:
    uint8_t  alpha = 0.1;
    uint16_t prevValue = 0;

  public:
    uint32_t Smoothing(uint16_t data);

};

uint32_t ExponentialSmoothing::Smoothing(uint16_t data)
{
  uint32_t smoothValue = (alpha * data) + (1 - alpha) * (prevValue);
  prevValue = smoothValue;
  return smoothValue;
}


#endif
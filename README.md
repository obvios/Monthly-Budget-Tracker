# Monthly-Budget-Tracker

## Purpose
To simplify tracking how much you spend and earn in a month, giving you an instant net view of your monthly budget.

## How does it work?
### Subtracting Expenses
* **Method 1:** The application can read your receipts by using a webcam and pointing it to a receipt. Once the receipt is detected you can press the 'q' key and and the image of the receipt will be processed and transformed using the OpenCV library and read using the Tessereact OCR API. Once the total is extracted, it will be deducted from your budget for the selected month.
* **Method 2:** The application allows users to manually input expenses to the month of their choosing.

### Adding income
* Currently, only manual input of income is supported. Soon you will be able to can scan your checks :)

## Example images from program
<img src="https://github.com/obvios/Monthly-Budget-Tracker/blob/master/ComputerVision_proj/Images/ExampleData.jpg" width="250" height="250">
                                                                                                                                          
![Example Data](https://github.com/obvios/Monthly-Budget-Tracker/blob/master/ComputerVision_proj/Images/ExampleData.jpg | width=250 | height=250)

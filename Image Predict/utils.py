# Importing Necessary Libraries
import tensorflow as tf
import numpy as np
from PIL import Image


# Cleaning image    
def clean_image(image):
    # Convert image to numpy array
    image = np.array(image)
    
    # Resize the image
    image = np.array(Image.fromarray(
        image).resize((512, 512), Image.Resampling.LANCZOS))
        
    # Normalize the image
    image = image / 255.0
    
    # Add batch dimensions and ensure 3 channels
    image = image[np.newaxis, :, :, :3]
    
    return image
    
    
def get_prediction(model, image):
    # Predict from the image
    predictions = model.predict(image)
    predictions_arr = np.array(np.argmax(predictions))
    
    return predictions, predictions_arr
    

# Making the final results 
def make_results(predictions, predictions_arr):
    
    result = {}
    if int(predictions_arr) == 0:
        result = {"status": " is Healthy ",
                    "prediction": f"{int(predictions[0][0].round(2)*100)}%"}
    elif int(predictions_arr) == 1:
        result = {"status": ' has Multiple Diseases ',
                    "prediction": f"{int(predictions[0][1].round(2)*100)}%"}
    elif int(predictions_arr) == 2:
        result = {"status": ' has Rust ',
                    "prediction": f"{int(predictions[0][2].round(2)*100)}%"}
    elif int(predictions_arr) == 3:
        result = {"status": ' has Scab ',
                    "prediction": f"{int(predictions[0][3].round(2)*100)}%"}
    return result
import numpy as np

volt = [4002,4302,4103]
current = [1015, 24, 400]

def computeSoh(voltSamples: list, currentSamples: list, debug: bool = False):
    # Create the A matrix
    A_matrix = np.ones(shape=(len(voltSamples),2))
    A_matrix[:, 1] = currentSamples
    # Create the B vector
    B_vector = np.array(object=voltSamples)
    
    A_transpose = np.transpose(A_matrix)
    # Matrix multiplication
    step1 = np.matmul(A_transpose, A_matrix)
    
    # Compute inverse
    try:
        step2_inv = np.linalg.inv(step1)
    except np.linalg.LinAlgError:
        print("Error computing inverse of matrix for computing SOH.")
    
    result1 = np.matmul(step2_inv, A_transpose)
    result = np.dot(result1, B_vector)
    if (debug):
        print("A Matrix")
        print(A_matrix)
        print("B_vector")
        print(B_vector)
        print("A_t Matrix")
        print(A_transpose)
        print("Step 1 : A_t * A")
        print(step1)
        print("Step2: inverse step 1")
        print(step2_inv)
        print("Final Step 1: step_2_inv * A_t")
        print(result1)
        print("Final Step 2: result1 * B")
        print(result)
    return result
    
res = computeSoh(volt, current, True)



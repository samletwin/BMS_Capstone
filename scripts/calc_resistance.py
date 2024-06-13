current_gain = 20
Ic = 1

Ib = Ic/current_gain

Rb = (3.3-0.7)/Ib

print(Rb)

current_gain = 40
# Ic = 0.5

Ib = Ic/current_gain

V_be = -Rb*Ib+3.3
print(V_be)
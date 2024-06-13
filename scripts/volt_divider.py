
V_source = 4.2
V_max = 2.4
V_min = 0
R2 = 10000
# R3 = 28000

 # Vout = Vsource * R_3/(R2+R_3)
# Vsource = 2.6*(R2+R3)/R3
# print(Vsource)
# R_3*Vout+R2*Vout = Vsource*R3
# R3*(vout-vsource) = -r2*vout
R3 = -R2*V_max/(V_max-V_source) # = 28k approx
# R3 = 28000
print(R3)

V_max = V_min

Vout = V_source * R3/(R2+R3)
print(Vout)
Vout = 3 * R3/(R2+R3)
print(Vout)
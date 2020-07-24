import matplotlib.pyplot as plt
from numpy.random import randint
from numpy import linspace

# sea la funcion f(x) = x^2 + 1   f'(x) = 2x


# parametros iniciales

xInicio = 3
learningRate = 0.25
numIteracion = 50

iteracion = []                     # iteracion correpsondiente
y = []                             # valor y obtenido

x = xInicio

for i in range(numIteracion ):
    print('*_____________*')
    print('iteracion : ', str(i + 1))

    grad = 2 * x                    # calculo del gradiente como valor

    x = x - learningRate * grad     # actualizar el valor de x (abscissa)

    y.append(x**2 + 1)
    iteracion.append(i+1)

    print('x = ', str(x), ', y =', str(x**2 + 1))



plt.subplot(1,2,1)
plt.plot(iteracion,y)
plt.xlabel('Iteración')
plt.ylabel('y')

X = linspace(-5,5,100)
Y = X**2 + 1
plt.subplot(1,2,2)
plt.plot(X,Y,0.0,1.0,'ro')
plt.xlabel('x')
plt.ylabel('y')

plt.show()
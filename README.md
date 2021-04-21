PAV - P3: detección de pitch
============================

Esta práctica se distribuye a través del repositorio GitHub [Práctica 3](https://github.com/albino-pav/P3).
Siga las instrucciones de la [Práctica 2](https://github.com/albino-pav/P2) para realizar un `fork` de la
misma y distribuir copias locales (*clones*) del mismo a los distintos integrantes del grupo de prácticas.

Recuerde realizar el *pull request* al repositorio original una vez completada la práctica.

Ejercicios básicos
------------------

- Complete el código de los ficheros necesarios para realizar la detección de pitch usando el programa
  `get_pitch`.

   * Complete el cálculo de la autocorrelación e inserte a continuación el código correspondiente.

   * Inserte una gŕafica donde, en un *subplot*, se vea con claridad la señal temporal de un segmento de
     unos 30 ms de un fonema sonoro y su periodo de pitch; y, en otro *subplot*, se vea con claridad la
	 autocorrelación de la señal y la posición del primer máximo secundario.

	 NOTA: es más que probable que tenga que usar Python, Octave/MATLAB u otro programa semejante para
	 hacerlo. Se valorará la utilización de la librería matplotlib de Python.

   La autocorrelación nos queda de la forma siguiente:

   <p align="center">
   <img src="img/pygraph1.png" width="540" align="center">
  </p>

  Para encontrar el período de la gráfica, lo hemos programado buscando picos en el tiempo. Este método no es muy fiable, ya que a la mínima que tuvieramos distorsión la gráfica en tiempo oscilaría:

  ```py
  peaks = sig.find_peaks(data_in)
  peaks = peaks[0] 
  first_peak = peaks[0] 
  fm=16000
  #find second peak (90% of the maximum)
  max = data_in[first_peak]
  for i in range(len(peaks)-1):
    if((data_in[peaks[i+1]])> max*0.9):
      second_peak = peaks[i+1]
      break
  ```
  El resto del código se encuentra en `plt_aut.py`

   * Determine el mejor candidato para el periodo de pitch localizando el primer máximo secundario de la
     autocorrelación. Inserte a continuación el código correspondiente.

  El código utilizado es:

  ```cpp
  for (iR = r.begin() + npitch_min; iR < r.begin() + npitch_max; iR++)
  {
    if (*iR > *iRMax)
    {
      iRMax = iR;
    }
  }
  ```

   * Implemente la regla de decisión sonoro o sordo e inserte el código correspondiente.
  
  Una primera regla de decisión ha sido usar un umbral para r[1]/r[0]:
   
  ```cpp
    if(r1norm<k1)
    return true;
  else
    return false;
  ```

  Como en la práctica anterior, el valor óptimo de k1 lo hemos calculado usando un bucle con un script de shell. Para ello también hemos tenido que modificar `pitch_evaluate.cpp` y los docopts de ambos programas.

- Una vez completados los puntos anteriores, dispondrá de una primera versión del detector de pitch. El 
  resto del trabajo consiste, básicamente, en obtener las mejores prestaciones posibles con él.

  * Utilice el programa `wavesurfer` para analizar las condiciones apropiadas para determinar si un
    segmento es sonoro o sordo. 
	
	  - Inserte una gráfica con la detección de pitch incorporada a `wavesurfer` y, junto a ella, los 
	    principales candidatos para determinar la sonoridad de la voz: el nivel de potencia de la señal
		(r[0]), la autocorrelación normalizada de uno (r1norm = r[1] / r[0]) y el valor de la
		autocorrelación en su máximo secundario (rmaxnorm = r[lag] / r[0]).

		Puede considerar, también, la conveniencia de usar la tasa de cruces por cero.

	    Recuerde configurar los paneles de datos para que el desplazamiento de ventana sea el adecuado, que
		en esta práctica es de 15 ms.

     Si calculamos la potencia, autocorrelación de uno y autocorrelación del máximo secundario obtenemos:
    <p align="center">
    <img src="img/wavesurfergraph1.png" width="720" align="center">
    </p>
   
    *Potencia, r[1]/r[0], r[1]/rmax, pitch detectado por wavesurfer y señal de rl001*


      - Use el detector de pitch implementado en el programa `wavesurfer` en una señal de prueba y compare
	    su resultado con el obtenido por la mejor versión de su propio sistema.  Inserte una gráfica
		ilustrativa del resultado de ambos detectores.
  
  * Optimice los parámetros de su sistema de detección de pitch e inserte una tabla con las tasas de error
    y el *score* TOTAL proporcionados por `pitch_evaluate` en la evaluación de la base de datos 
	`pitch_db/train`..

  Para la optimización, hemos creado el siguiente script:

  ```bash
  GETF0="get_pitch"
  EVAL="pitch_evaluate"

  lower_index_bound=80                    #lower bound of the for loop
  upper_index_bound=100                   #upper bound of the for loop
  offset=0                                #offset of the variable we want to iterate
  div=100                                 #controls the step size of the variable when we iterate it (=2 -> /2)
  for index in $(seq $lower_index_bound $upper_index_bound); do #vary $index from (lower_index_bound) to (upper_index_bound)
    k0=$(bc <<<"scale=5; $offset+$index/$div")    #stores offset+$index/div in k0 (uses basic calculator (bc) since shell doesn't seem to support floating point by default)
    for fwav in pitch_db/train/*.wav; do
     ff0=${fwav/.wav/.f0}
    $GETF0 $fwav $ff0 -0 $k0> /dev/null || (echo "Error in $GETF0 $fwav $ff0"; exit 1)
    done
    res=`$EVAL -l pitch_db/train/*f0ref || (echo "Error in $GETF0 $fwav $ff0"; exit 1)`
    echo "$k0 $res"
  done
  ```
  Como la última vez, simplemente itera sobre el script ya hecho `run_get_pitch`, y esto lo hace variando `k0` según queramos. En este caso está modificando la variable `k0` (`-0`). Para obtener los resultados en un fichero de texto, solo hemos de añadir `>nombre_out.txt` al final del comando cuando llamamos al script. Como `pitch_evaluate` está modificado de tal manera que solo imprima el total al usar `-l`, en nuestro fichero de salida nos quedará una primera columna con los valores de `k0` y una segunda con los resultados correspondientes a ese valor.

   * Inserte una gráfica en la que se vea con claridad el resultado de su detector de pitch junto al del
     detector de Wavesurfer. Aunque puede usarse Wavesurfer para obtener la representación, se valorará
	 el uso de alternativas de mayor calidad (particularmente Python).
   
   Nos queda lo siguiente:

    <p align="center">
    <img src="img/pygraph3.png" width="620" align="center">
    </p>
   

Ejercicios de ampliación
------------------------

- Usando la librería `docopt_cpp`, modifique el fichero `get_pitch.cpp` para incorporar los parámetros del
  detector a los argumentos de la línea de comandos.
  
  Esta técnica le resultará especialmente útil para optimizar los parámetros del detector. Recuerde que
  una parte importante de la evaluación recaerá en el resultado obtenido en la detección de pitch en la
  base de datos.

  * Inserte un *pantallazo* en el que se vea el mensaje de ayuda del programa y un ejemplo de utilización
    con los argumentos añadidos.

Por un lado, en el programa principal `get_pitch` hemos añadido lo siguiente:

<p align="center">
   <img src="img/docopt1.png" width="480">
</p>

Además, para faciltar la lectura y guardado en los valores generados con nuestro script de shell `opt_get_pitch`, hemos añadido la opción `-l` en `pitch_evaluate`, la cual solo imprime el resultado final del resumen:

<p align="center">
   <img src="img/docopt2.png" width="480">
</p>

- Implemente las técnicas que considere oportunas para optimizar las prestaciones del sistema de detección
  de pitch.

  Entre las posibles mejoras, puede escoger una o más de las siguientes:

  * Técnicas de preprocesado: filtrado paso bajo, *center clipping*, etc.
  * Técnicas de postprocesado: filtro de mediana, *dynamic time warping*, etc.
  * Métodos alternativos a la autocorrelación: procesado cepstral, *average magnitude difference function*
    (AMDF), etc.
  * Optimización **demostrable** de los parámetros que gobiernan el detector, en concreto, de los que
    gobiernan la decisión sonoro/sordo.
  * Cualquier otra técnica que se le pueda ocurrir o encuentre en la literatura.

  Encontrará más información acerca de estas técnicas en las [Transparencias del Curso](https://atenea.upc.edu/pluginfile.php/2908770/mod_resource/content/3/2b_PS%20Techniques.pdf)
  y en [Spoken Language Processing](https://discovery.upc.edu/iii/encore/record/C__Rb1233593?lang=cat).
  También encontrará más información en los anexos del enunciado de esta práctica.

  Incluya, a continuación, una explicación de las técnicas incorporadas al detector. Se valorará la
  inclusión de gráficas, tablas, código o cualquier otra cosa que ayude a comprender el trabajo realizado.

  También se valorará la realización de un estudio de los parámetros involucrados. Por ejemplo, si se opta
  por implementar el filtro de mediana, se valorará el análisis de los resultados obtenidos en función de
  la longitud del filtro.
   

Evaluación *ciega* del detector
-------------------------------

Antes de realizar el *pull request* debe asegurarse de que su repositorio contiene los ficheros necesarios
para compilar los programas correctamente ejecutando `make release`.

Con los ejecutables construidos de esta manera, los profesores de la asignatura procederán a evaluar el
detector con la parte de test de la base de datos (desconocida para los alumnos). Una parte importante de
la nota de la práctica recaerá en el resultado de esta evaluación.

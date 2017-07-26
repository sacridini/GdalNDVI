#include <iostream>
#include <gdal_priv.h>

#if 1
	#define LOG(msg) std::cout << msg << std::endl;
#endif

using namespace std;

int main() {
	LOG("--> Gerando NDVI <--");
	LOG("Iniciando processo.");

    GDALAllRegister();

    LOG("############################");

	const char* nirFilePath = "C:\\Users\\eduardo\\Documents\\LT05_L1TP_217076_20110813_20161007_01_T1_B4.TIF";
	const char* redFilePath = "C:\\Users\\eduardo\\Documents\\LT05_L1TP_217076_20110813_20161007_01_T1_B3.TIF";
	const char* outFilePath = "C:\\Users\\eduardo\\Documents\\ndvi.tif";

    LOG("Abrindo arquivos:");

	cout << "Banda do Infravermelho: " << nirFilePath << endl;
	cout << "Banda do Vermelho: " << redFilePath << endl;
	cout << "Endereço do NDVI que sera gerado: " << outFilePath << endl;

	LOG("############################");

    LOG("Criando datasets...");
	GDALDataset *red, *nir, *ndvi;
	GDALDriver *geoTiff;
	int nRows, nCols;
	double noData;
	double transform[6];

	// Load input images (NIR and RED bands)
	LOG("############################");
    LOG("Carregando as imagens nos datasets...");
	LOG("############################");
	nir = (GDALDataset*)GDALOpen(nirFilePath, GA_ReadOnly);
	red = (GDALDataset*)GDALOpen(redFilePath, GA_ReadOnly);
    LOG("Datasets carregados com sucesso!");

	// Get raster info to use as parameters
    LOG("############################");
    LOG("Propriedades dos datasets:");
	LOG("############################");
	nRows = nir->GetRasterBand(1)->GetYSize();
	nCols = nir->GetRasterBand(1)->GetXSize();
	noData = nir->GetRasterBand(1)->GetNoDataValue();
	nir->GetGeoTransform(transform);
	cout << "Numero de linhas: " << nRows << endl;
	cout << "Numero de colunas: " << nCols << endl;
	for (int i = 0; i < 6; i++)
	{
        LOG(transform[i]);
	}

	// Cria a imagem NDVI - Create NDVI source imagem
	geoTiff = GetGDALDriverManager()->GetDriverByName("GTiff");
	ndvi = geoTiff->Create(outFilePath, nCols, nRows, 1, GDT_Float32, NULL);
	ndvi->SetGeoTransform(transform);
	ndvi->GetRasterBand(1)->SetNoDataValue(noData);

	// Reserva memoria para os buffers
    LOG("Alocando memoria para os buffers");
	float *nir_Row = (float*)CPLMalloc(sizeof(float)*nCols);
	float *red_Row = (float*)CPLMalloc(sizeof(float)*nCols);
	float *upper_Row = (float*)CPLMalloc(sizeof(float)*nCols);
	float *lower_Row = (float*)CPLMalloc(sizeof(float)*nCols);
	float *ndvi_Row = (float*)CPLMalloc(sizeof(float)*nCols);

    LOG("Gerando NDVI. Aguarde...");

	for (int i = 0; i < nRows; i++)
	{
		nir->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, nir_Row, nCols, 1, GDT_Float32, 0, 0);
		red->GetRasterBand(1)->RasterIO(GF_Read, 0, i, nCols, 1, red_Row, nCols, 1, GDT_Float32, 0, 0);
		for (int j = 0; j < nCols; j++)
		{
			if (nir_Row[j] == noData)
			{
				upper_Row[j] = noData;
				lower_Row[j] = noData;
				ndvi_Row[j] = noData;
			}
			else
			{
				upper_Row[j] = nir_Row[j] - red_Row[j];
				lower_Row[j] = nir_Row[j] + red_Row[j];
				ndvi_Row[j] = upper_Row[j] / lower_Row[j];
			}
		}
		ndvi->GetRasterBand(1)->RasterIO(GF_Write, 0, i, nCols, 1, ndvi_Row, nCols, 1, GDT_Float32, 0, 0);
	}

	// delete buffers
	CPLFree(nir_Row);
	CPLFree(red_Row);
	CPLFree(upper_Row);
	CPLFree(lower_Row);
	CPLFree(ndvi_Row);

	// delete datasets
	GDALClose(red);
	GDALClose(nir);
	GDALClose(ndvi);
	GDALDestroyDriverManager();

    LOG("############################");
    LOG("--> NDVI gerado com sucesso! <--");
	LOG("############################");
	LOG("Processo concluido com sucesso.");

	system("PAUSE");
	return 0;
}
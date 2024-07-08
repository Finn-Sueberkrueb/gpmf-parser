#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <dirent.h>
#include "../GPMF_parser.h"
#include "GPMF_mp4reader.h"
#include "../GPMF_utils.h"

#define SHOW_THIS_FOUR_CC_GYRO STR2FOURCC("GYRO")
#define SHOW_THIS_FOUR_CC_ACCL STR2FOURCC("ACCL")
#define SHOW_THIS_FOUR_CC_GRAV STR2FOURCC("GRAV")

GPMF_ERR readMP4FileAndWriteCSV(char *filename);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <input_directory>\n", argv[0]);
        return -1;
    }

    struct dirent *entry;
    DIR *dp = opendir(argv[1]);
    if (dp == NULL)
    {
        perror("opendir");
        return -1;
    }

    while ((entry = readdir(dp)))
    {
        if (entry->d_type == DT_REG)
        {
            // Check if file has .MP4 extension
            char *dot = strrchr(entry->d_name, '.');
            if (dot && strcmp(dot, ".MP4") == 0)
            {
                char filepath[1024];
                snprintf(filepath, sizeof(filepath), "%s/%s", argv[1], entry->d_name);

                GPMF_ERR ret = readMP4FileAndWriteCSV(filepath);

                if (ret != GPMF_OK)
                {
                    printf("Error reading MP4 file or writing CSV file for %s\n", entry->d_name);
                }
            }
        }
    }

    closedir(dp);
    return 0;
}

GPMF_ERR readMP4FileAndWriteCSV(char *filename)
{
    GPMF_ERR ret = GPMF_OK;
    GPMF_stream metadata_stream = {0}, *ms = &metadata_stream;
    double metadatalength;
    uint32_t *payload = NULL;
    uint32_t payloadsize = 0;
    size_t payloadres = 0;

    // Create the CSV filename based on the MP4 filename
    char csv_filename[1024];
    strncpy(csv_filename, filename, sizeof(csv_filename) - 5);
    csv_filename[sizeof(csv_filename) - 5] = '\0'; // Ensure null termination
    char *dot = strrchr(csv_filename, '.');
    if (dot)
    {
        *dot = '\0';
    }
    strcat(csv_filename, ".csv");

    // Open the CSV file in write mode
    FILE *csv_file = fopen(csv_filename, "w");
    if (!csv_file)
    {
        printf("Error opening CSV file for writing\n");
        return GPMF_ERROR_MEMORY;
    }

    // Write the CSV header
    fprintf(csv_file, "Time,Type,X,Y,Z\n");

    size_t mp4handle = OpenMP4Source(filename, MOV_GPMF_TRAK_TYPE, MOV_GPMF_TRAK_SUBTYPE, 0);
    if (mp4handle == 0)
    {
        printf("Error: %s is an invalid MP4/MOV or it has no GPMF data\n\n", filename);
        fclose(csv_file);
        return GPMF_ERROR_BAD_STRUCTURE;
    }

    metadatalength = GetDuration(mp4handle);

    if (metadatalength > 0.0)
    {
        uint32_t index, payloads = GetNumberPayloads(mp4handle);

        for (index = 0; index < payloads; index++)
        {
            double in = 0.0, out = 0.0; // times
            payloadsize = GetPayloadSize(mp4handle, index);
            payloadres = GetPayloadResource(mp4handle, payloadres, payloadsize);
            payload = GetPayload(mp4handle, payloadres, index);
            if (payload == NULL)
                goto cleanup;

            ret = GetPayloadTime(mp4handle, index, &in, &out);
            if (ret != GPMF_OK)
                goto cleanup;

            ret = GPMF_Init(ms, payload, payloadsize);
            if (ret != GPMF_OK)
                goto cleanup;

            while (GPMF_OK == GPMF_FindNext(ms, GPMF_KEY_STREAM, GPMF_RECURSE_LEVELS | GPMF_TOLERANT))
            {
                if (GPMF_OK != GPMF_SeekToSamples(ms))
                    continue;

                char *rawdata = (char *)GPMF_RawData(ms);
                uint32_t key = GPMF_Key(ms);
                GPMF_SampleType type = GPMF_Type(ms);
                uint32_t samples = GPMF_Repeat(ms);
                uint32_t elements = GPMF_ElementsInStruct(ms);

                if (samples)
                {
                    uint32_t buffersize = samples * elements * sizeof(double);
                    double *tmpbuffer = (double *)malloc(buffersize);

                    if (tmpbuffer)
                    {
                        if (GPMF_OK == GPMF_ScaledData(ms, tmpbuffer, buffersize, 0, samples, GPMF_TYPE_DOUBLE))
                        {
                            double *ptr = tmpbuffer;
                            double sample_interval = (out - in) / samples;

                            for (uint32_t i = 0; i < samples; i++)
                            {
                                double sample_time = in + i * sample_interval;
                                if (key == SHOW_THIS_FOUR_CC_ACCL || key == SHOW_THIS_FOUR_CC_GYRO)
                                {
                                    fprintf(csv_file, "%.6f,%c%c%c%c,%.3f,%.3f,%.3f\n", sample_time, PRINTF_4CC(key), ptr[0], ptr[1], ptr[2]);
                                }
                                ptr += elements;
                            }
                        }
                        free(tmpbuffer);
                    }
                }
            }

            GPMF_ResetState(ms);
        }
    }

cleanup:
    if (payloadres)
        FreePayloadResource(mp4handle, payloadres);
    if (ms)
        GPMF_Free(ms);
    CloseSource(mp4handle);
    fclose(csv_file);

    return ret;
}